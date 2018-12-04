
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class for Unix.
//  https://computing.llnl.gov/tutorials/pthreads/
//  http://publib.boulder.ibm.com/infocenter/systems/index.jsp?topic=/com.ibm.aix.genprogc/doc/genprogc/mutexes.htm
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/UnixMutex.h"
#include "Usul/Strings/Format.h"

#include <stdexcept>

#include <cerrno>
#include <iostream>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Class to manage the initialization and destruction of a mutex attribute.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct MutexAttribute
  {
    MutexAttribute ( int type ) : _attr()
    {
      // Have to call this first. If you do not, then most of the time it will
      // still work as expected, which makes it a difficult error to catch.
      int result = ::pthread_mutexattr_init ( &_attr );
      if ( 0 != result )
      {
	throw std::runtime_error 
	  ( Usul::Strings::format 
	    ( "Error 1440030471: Failed to initialize mutex attribute, ",
	      "pthread_mutexattr_init() returned ", result ) );
      }

      // Set the mutex attribute.
      result = ::pthread_mutexattr_settype ( &_attr, type );
      if ( 0 != result )
      {
	throw std::runtime_error 
	  ( Usul::Strings::format 
	    ( "Error 5285206230: Failed to set mutex attribute, ",
	      "pthread_mutexattr_settype() returned ", result ) );
      }
    }

    ~MutexAttribute()
    {
      ::pthread_mutexattr_destroy ( &_attr );
    }

    pthread_mutexattr_t &attr()
    {
      return _attr;
    }

  private:

    pthread_mutexattr_t _attr;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make one mutex attribute.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  MutexAttribute mutexAttribute ( PTHREAD_MUTEX_RECURSIVE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

UnixMutex::UnixMutex() : _mutex()
{
  Detail::MutexAttribute &ma ( Detail::mutexAttribute );
  const int result ( ::pthread_mutex_init ( &_mutex, &ma.attr() ) );
  if ( 0 != result )
  {
    throw std::runtime_error 
      ( Usul::Strings::format 
	( "Error 8891501470: Failed to initialize mutex, ",
	  "pthread_mutex_init() returned ", result ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

UnixMutex::~UnixMutex()
{
  if ( 0 != ::pthread_mutex_destroy ( &_mutex ) )
  {
    std::cout << "Error 5792564950: Failed to destroy mutex \n" << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void UnixMutex::lock()
{
  if ( 0 != ::pthread_mutex_lock ( &_mutex ) )
  {
    throw std::runtime_error ( "Error 3525044930: Failed to lock mutex" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void UnixMutex::unlock()
{
  if ( 0 != ::pthread_mutex_unlock ( &_mutex ) )
  {
    throw std::runtime_error ( "Error 9919729800: Failed to unlock mutex" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to lock the mutex.  Will return true if the lock has been acquired.
//
///////////////////////////////////////////////////////////////////////////////

bool UnixMutex::trylock()
{
  return ( 0 == ::pthread_mutex_trylock ( &_mutex ) );
}
