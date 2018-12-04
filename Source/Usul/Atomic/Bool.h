
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Atomic class for bool.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ATOMIC_BOOL_CLASS_H_
#define _USUL_ATOMIC_BOOL_CLASS_H_

#include "Usul/Config/Config.h"
#ifdef USUL_USE_TBB_ATOMIC_CLASS
#include "tbb/atomic.h"
#else
#include "Usul/Atomic/Object.h"
#endif


namespace Usul {
namespace Atomic {

#ifdef USUL_USE_TBB_ATOMIC_CLASS

struct Bool : public tbb::atomic < bool >
{
  typedef bool ValueType;
  typedef Bool ThisType;
  typedef tbb::atomic < ValueType > BaseClass;

  Bool() : BaseClass()
  {
  }

  Bool ( ValueType value ) : BaseClass()
  {
    BaseClass::operator = ( value );
  }
};

#else

struct Bool : public Usul::Atomic::Object < bool >
{
  // Typedefs
  typedef bool ValueType;
  typedef Bool ThisType;
  typedef Usul::Atomic::Object < ValueType > BaseClass;

  // Constructors and destructor.
  Bool() : BaseClass()
  {
  }

  Bool ( ValueType v ) : BaseClass ( v )
  {
  }

private:

  // Do not use these.
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Do not use these.
  ValueType &getReference() { return BaseClass::getReference(); }
  Mutex &mutex() { return BaseClass::mutex(); }
};

#endif

} // namespace Atomic
} // namespace Usul


#endif // _USUL_ATOMIC_BOOL_CLASS_H_
