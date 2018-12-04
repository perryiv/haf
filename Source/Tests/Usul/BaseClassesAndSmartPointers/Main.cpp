
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for Usul's base classes and smart-pointers.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Base/Ref/Careful.h"
#include "Usul/Base/Ref/MultiThreaded.h"
#include "Usul/Base/Ref/SingleThreaded.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/System/Clock.h"
#include "Usul/Threads/FakeMutex.h"

#include <iomanip>
#include <limits>
#include <sstream>
#include <string>


template < class T > struct TypeName
{
  static std::string get ( const std::string &defaultName = std::string ( "unknown type" ) )
  {
    const std::type_info &info ( typeid ( T ) );
    const std::string name ( ( 0x0 == info.name() ) ? defaultName : std::string ( info.name() ) );
    return name;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Generic reference-counted class.
//
///////////////////////////////////////////////////////////////////////////////

template < class BaseClassType > class MyGeneric : public BaseClassType
{
public:

  typedef MyGeneric < BaseClassType > ThisType;
  typedef BaseClassType BaseClass;

  USUL_DECLARE_REF_POINTERS ( MyGeneric );

  MyGeneric() : BaseClass()
  {
  }

  void mutableFunction(){}
  void constantFunction() const {}

protected:

  virtual ~MyGeneric()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test the smart-pointer.
//
///////////////////////////////////////////////////////////////////////////////

template < class PointerType > struct TestPointer
{
  typedef PointerType Pointer;
  typedef typename Pointer::element_type ObjectType;

  static void test ( bool testSettingToNull, unsigned long num )
  {
    _mutableTest ( testSettingToNull );
    _constTest();
    _referenceTest ( num );
  }

protected:

  static void _referenceTest ( unsigned long num )
  {
    // Make sure we don't overflow the reference count type.
    const unsigned long inner ( std::numeric_limits<unsigned char>::max() - 2 );

    for ( unsigned int j = 0; j < num; ++j )
    {
      Pointer ptr ( new ObjectType );

      for ( unsigned long i = 0; i < inner; ++i )
      {
        Pointer temp ( ptr );
        ptr->ref();
      }

      for ( unsigned long i = 0; i < inner; ++i )
      {
        Pointer temp ( ptr );
        ptr->unref();
      }
    }
  }

  static void _mutableTest ( bool testSettingToNull )
  {
    Pointer a ( new ObjectType );
    Pointer b = Pointer ( new ObjectType ); // No assignment to raw pointers.
    Pointer c ( a );

    a = b;
    b = c;

    a->mutableFunction();
    b->mutableFunction();
    c->mutableFunction();

    a->constantFunction();
    b->constantFunction();
    c->constantFunction();

    if ( testSettingToNull )
    {
      a = Pointer ( 0x0 );
      b = Pointer ( 0x0 );
    }
  }

  static void _constTest()
  {
    const Pointer a ( new ObjectType );
    const Pointer b = Pointer ( new ObjectType ); // No assignment to raw pointers.
    const Pointer c ( a );

    a->constantFunction();
    b->constantFunction();
    c->constantFunction();
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test the smart-pointers.
//
///////////////////////////////////////////////////////////////////////////////

template < class BaseClassType > struct TestPointers
{
  typedef BaseClassType BaseClass;
  typedef MyGeneric < BaseClass > ObjectType;

  static void test ( unsigned long num )
  {
    TestPointer<typename ObjectType::RefPtr>::test ( true, num );
    TestPointer<typename ObjectType::ValidRefPtr>::test ( false, num );
    TestPointer<typename ObjectType::ValidAccessRefPtr>::test ( true, num );

    // As expected, this causes a leak.
    //TestPointer<typename ObjectType::NoDeleteRefPtr>::test ( true, num );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

template < class BaseClassType > struct Test
{
  static void test ( unsigned long num )
  {
    const Usul::Types::UInt64 start ( Usul::System::Clock::milliseconds() );

    TestPointers<BaseClassType>::test ( num );

    const Usul::Types::UInt64 stop ( Usul::System::Clock::milliseconds() );
    std::ostringstream out;
    out << std::setw (  7 ) << stop - start << " ms "
        << std::setw ( 10 ) << num << " iterations "
        << std::setw ( 30 ) << TypeName<BaseClassType>::get() << ' '
        << '\n';
    std::cout << out.str() << std::flush;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void test ( unsigned long num )
{
  Test < Usul::Base::Ref::Careful >::test ( num );
  Test < Usul::Base::Ref::MultiThreaded >::test ( num );
  Test < Usul::Base::Ref::SingleThreaded >::test ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  bool result ( 1 );
  USUL_TRY_BLOCK
  {
    ::test ( 10000 );
    result = 0;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3740981865" );
  return result;
}
