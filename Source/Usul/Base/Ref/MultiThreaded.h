
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for thread-safe reference counting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_MULTI_THREADED_CLASS_H_
#define _USUL_BASE_MULTI_THREADED_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Integer.h"


namespace Usul {
namespace Base {
namespace Ref {


class MultiThreaded
{
public:

  typedef unsigned long RefCount;

  void ref()
  {
    ++_refCount;
  }

  void unref ( bool allowDelete = true )
  {
    const RefCount count ( --_refCount );
    if ( ( 0 == count ) && ( true == allowDelete ) )
    {
      delete this;
    }
  }
  
  RefCount refCount() const
  {
    return _refCount;
  }

protected:


  MultiThreaded() : _refCount ( 0 )
  {
  }

  MultiThreaded ( const MultiThreaded &r ) : _refCount ( 0 )
  {
  }

  MultiThreaded &operator = ( const MultiThreaded &r )
  {
    return *this;
  }

  virtual ~MultiThreaded()
  {
  }

private:

  typedef Usul::Atomic::Integer<RefCount> AtomicRefCount;
  AtomicRefCount _refCount;
};


} // namespace Ref
} // namespace Base
} // namespace Usul


#endif // _USUL_BASE_MULTI_THREADED_CLASS_H_
