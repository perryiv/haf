
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Atomic class for integers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ATOMIC_INTEGER_CLASS_H_
#define _USUL_ATOMIC_INTEGER_CLASS_H_

#include "Usul/Config/Config.h"
#ifdef USUL_USE_TBB_ATOMIC_CLASS
#include "tbb/atomic.h"
#else
#include "Usul/Atomic/Object.h"
#endif


namespace Usul {
namespace Atomic {

#ifdef USUL_USE_TBB_ATOMIC_CLASS

template < class IntegerType > struct Integer : public tbb::atomic < IntegerType >
{
  typedef IntegerType ValueType;
  typedef Integer < ValueType > ThisType;
  typedef tbb::atomic < ValueType > BaseClass;

  Integer() : BaseClass()
  {
  }

  Integer ( ValueType value ) : BaseClass()
  {
    BaseClass::operator = ( value );
  }
};

#else

template < class IntegerType > struct Integer : public Usul::Atomic::Object < IntegerType >
{
  // Typedefs
  typedef IntegerType ValueType;
  typedef Integer < ValueType > ThisType;
  typedef Usul::Atomic::Object < ValueType > BaseClass;

  // Constructors and destructor.
  Integer() : BaseClass()
  {
  }

  Integer ( ValueType v ) : BaseClass ( v )
  {
  }

  ValueType operator ++ ()
  {
    Guard guard ( this->mutex() );
    ++(this->getReference());
    return this->getReference();
  }

  ValueType operator ++ ( int )
  {
    Guard guard ( this->mutex() );
    ValueType value ( this->getReference() );
    ++(this->getReference());
    return value;
  }

  ValueType operator -- ()
  {
    Guard guard ( this->mutex() );
    --(this->getReference());
    return this->getReference();
  }

  ValueType operator -- ( int )
  {
    Guard guard ( this->mutex() );
    ValueType value ( this->getReference() );
    --(this->getReference());
    return value;
  }

private:

  // Do not use these.
  typedef typename BaseClass::Mutex Mutex;
  typedef typename BaseClass::Guard Guard;

  // Do not use these.
  ValueType &getReference() { return BaseClass::getReference(); }
  Mutex &mutex() { return BaseClass::mutex(); }
};

#endif

} // namespace Atomic
} // namespace Usul


#endif // _USUL_ATOMIC_INTEGER_CLASS_H_
