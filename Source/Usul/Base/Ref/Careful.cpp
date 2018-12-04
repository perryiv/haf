
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counted base class that checks for errors.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Base/Ref/Careful.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Guard.h"

#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <typeinfo>

using namespace Usul::Base::Ref;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to print output.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void print ( const std::string &s )
  {
    if ( false == s.empty() )
    {
      #ifdef __APPLE__
      ::fprintf( stderr, "%s\n", s.c_str() );
      ::fflush( stderr );
      #else
      std::cout << s << std::flush;
      #endif
      #ifdef _MSC_VER
      ::OutputDebugString ( s.c_str() );
      #endif
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Atomic counter of all instances.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef USUL_USE_INSTANCE_COUNTER

namespace Helper
{
  typedef Usul::Atomic::Integer < Usul::Types::UInt64 > InstanceCount;
  InstanceCount instanceCount;
}

namespace
{
  struct CheckInstanceCount
  {
    CheckInstanceCount(){}
    ~CheckInstanceCount()
    {
      if ( 0 != Helper::instanceCount )
      {
        Helper::print ( Usul::Strings::format ( "Number of objects remaining = ", Helper::instanceCount, '\n' ) );
        USUL_ASSERT ( false );
      }
    }
  } checkInstanceCount;
}

#define INSTANCE_COUNT_INCREMENT ++Helper::instanceCount
#define INSTANCE_COUNT_DECREMENT --Helper::instanceCount
#define INSTANCE_COUNT_GET Helper::instanceCount

#else

#define INSTANCE_COUNT_INCREMENT
#define INSTANCE_COUNT_DECREMENT
#define INSTANCE_COUNT_GET 0

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Simple instance manager.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef USUL_USE_INSTANCE_MANAGER

namespace Helper
{
  struct InstanceManager
  {
    typedef Usul::Base::Ref::Careful Careful;
    typedef std::map < const Careful *, std::string > Items;
    typedef Usul::Atomic::Container < Items > Data;
    InstanceManager() : _data(){}
    ~InstanceManager()
    {
      if ( false == _data.empty() )
      {
        USUL_ASSERT ( false ); // Heads up.
        Data::Guard guard ( _data.mutex() );
        Items &items ( _data.getReference() );
        Helper::print ( Usul::Strings::format ( items.size(), " objects remaining:\n" ) );
        for ( Items::const_iterator i = items.begin(); i != items.end(); ++i )
        {
          const Careful *address ( i->first );
          const std::string &type ( i->second );
          Helper::print ( Usul::Strings::format ( "\tAddress: ", address, ", Type: ", type, '\n' ) );
        }
      }
    }
    void add ( const Careful *c )
    {
      if ( 0x0 != c )
      {
        Data::Guard guard ( _data.mutex() );
        Items &items ( _data.getReference() );
        items[c] = c->typeName(); // Probably wrong.
      }
    }
    void update ( const Careful *c )
    {
      if ( 0x0 != c )
      {
        Data::Guard guard ( _data.mutex() );
        Items &items ( _data.getReference() );
        items[c] = c->typeName(); // Should be full object now.
      }
    }
    void remove ( const Careful *c )
    {
      Data::Guard guard ( _data.mutex() );
      Items &items ( _data.getReference() );
      items.erase ( c );
    }
    std::string get ( const Careful *c )
    {
      Data::Guard guard ( _data.mutex() );
      Items &items ( _data.getReference() );
      Items::const_iterator i ( items.find ( c ) );
      return ( ( items.end() == i ) ? std::string() : i->second );
    }
  private:
    Data _data;
  } instanceManager;
}

#define INSTANCE_MANAGER_ADD(the_object) \
  Helper::instanceManager.add ( the_object )

#define INSTANCE_MANAGER_UPDATE(the_object,the_ref_count) \
  if ( 1 == the_ref_count ) \
  { \
    Helper::instanceManager.update ( the_object ); \
  }

#define INSTANCE_MANAGER_REMOVE(the_object) \
  Helper::instanceManager.remove ( the_object )

#define INSTANCE_MANAGER_GET(the_object,the_type) \
  the_type = Helper::instanceManager.get ( the_object )

#else

#define INSTANCE_MANAGER_ADD(the_object)
#define INSTANCE_MANAGER_UPDATE(the_object,the_ref_count)
#define INSTANCE_MANAGER_REMOVE(the_object)
#define INSTANCE_MANAGER_GET(the_object,the_type)

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Careful::Careful() : 
  _refCount ( 0 ), 
  _objectNumber ( INSTANCE_COUNT_GET )
{
  INSTANCE_COUNT_INCREMENT;
  INSTANCE_MANAGER_ADD ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Careful::Careful ( const Careful &r ) : 
  _refCount ( 0 ), 
  _objectNumber ( INSTANCE_COUNT_GET )
{
  INSTANCE_COUNT_INCREMENT;
  INSTANCE_MANAGER_ADD ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Careful::~Careful()
{
  INSTANCE_COUNT_DECREMENT;

  bool deleteMe ( false );
  RefCount count ( 1 );
  {
    Guard guard ( _refCount.mutex );
    count = _refCount.count;
    deleteMe = _refCount.deleteMe;
  }

  if ( false == deleteMe )
  {
    std::string type;
    INSTANCE_MANAGER_GET ( this, type );
    std::cout << ( Usul::Strings::format ( 
      "Error 2592561907: Deleting object ", this, 
      " of type '", type, "'",
      " which has not been marked for deletion", 
      '\n' ) ) << std::flush;
  }

  if ( 0 != count )
  {
    // The static_cast is for when the count-type is unsigned char.
    // Otherwise, the conversion to string might be a blank.
    std::cout << ( Usul::Strings::format (
      "Error 2427831854: Deleting object ", this, " with reference count ", 
      static_cast < Usul::Types::UInt64 > ( count ) ), '\n' ) << std::flush;
  }

  // Do this last because you may need the entry above.
  INSTANCE_MANAGER_REMOVE ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  We define this function to make sure the compiler does not.
//
///////////////////////////////////////////////////////////////////////////////

Careful &Careful::operator = ( const Careful &r )
{
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void Careful::ref()
{
  if ( 0x0 == this )
  {
    throw std::runtime_error ( "Error 6722137890: Referencing a null object" );
  }

  Guard guard ( _refCount.mutex );

  // Are we already marked for deletion?
  if ( true == _refCount.deleteMe )
  {
    throw std::runtime_error ( Usul::Strings::format (
      "Error 3162870490: Address ", this, ", type '", this->typeName(), 
      "', attempting to reference an object that has been marked for deletion" ) );
  }

  // Will we roll over?
  if ( std::numeric_limits<RefCount>::max() == _refCount.count )
  {
    const std::type_info &info ( typeid ( RefCount ) );
    const std::string countType ( ( 0x0 == info.name() ) ? "unknown type" : info.name() );

    // The static_cast is for when the count-type is unsigned char.
    // Otherwise, the conversion to string might be a blank.
    throw std::runtime_error ( Usul::Strings::format (
      "Error 4544591930: Address ", this, ", type '", this->typeName(), 
      "', attempting to increment '", countType, "' with a value of ",
      static_cast < Usul::Types::UInt64 > ( _refCount.count ), " will overflow" ) );
  }

  ++_refCount.count;

  // If we just incremented to one then update the instance manager.
  INSTANCE_MANAGER_UPDATE ( this, _refCount.count );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void Careful::unref ( bool allowDelete )
{
  if ( 0x0 == this )
  {
    throw std::runtime_error ( "Error 6722137890: Dereferencing a null object" );
  }

  // Get the count data with a scoped lock.
  RefCount count ( 0 );
  bool deleteMe ( false );
  {
    // Lock the count.
    Guard guard ( _refCount.mutex );

    // Are we already marked for deletion?
    if ( true == _refCount.deleteMe )
    {
      throw std::runtime_error ( Usul::Strings::format (
        "Error 3760700259: Address ", this, ", type '", this->typeName(), 
        "', attempting to dereference an object that has been marked for deletion" ) );
    }

    // Will we roll over?
    if ( 0 == _refCount.count )
    {
      throw std::runtime_error ( Usul::Strings::format
        ( "Error 1107142984: Address ", this, ", type '", this->typeName(), 
        "', attempting to decrement a reference count that is already zero" ) );
    }

    // Decrement the count.
    --_refCount.count;

    // Set local variable holding the new count.
    count = _refCount.count;

    // If we've decremented to zero...
    if ( 0 == count )
    {
      if ( true == allowDelete )
      {
        // Set these while we still have the lock.
        _refCount.deleteMe = true;
        deleteMe = true;
      }
    }
  }

  // Now the lock has been released so we use the local variables.
  if ( ( 0 == count ) && ( true == allowDelete ) )
  {
    if ( false == deleteMe )
    {
      throw std::runtime_error ( Usul::Strings::format (
        "Error 3567337460: Address ", this, " of type '", this->typeName(),
        "' has not been marked for deletion" ) );
    }

    try
    {
      delete this;
    }

    catch ( std::exception &e )
    {
      const std::string message ( ( 0x0 == e.what() ) ? 
        Usul::Strings::format ( ", ", e.what() ) : std::string() );
      throw std::runtime_error ( Usul::Strings::format (
        "Error 3897452156: deleting address ", this, " of type '", this->typeName(), 
        "' caused an exception", message ) );
    }

    catch ( ... )
    {
      throw std::runtime_error ( Usul::Strings::format (
        "Error 2496190937: deleting address ", this, " of type '", this->typeName(),
        "' caused an exception" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the reference count.
//
///////////////////////////////////////////////////////////////////////////////

Careful::RefCount Careful::refCount() const
{
  Guard ( _refCount.mutex );
  return _refCount.count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the type name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Careful::typeName() const
{
  const std::type_info &info ( typeid ( *this ) );
  const char *name ( info.name() );
  const std::string type ( ( 0x0 == name ) ? "unknown type" : name );
  return type;
}
