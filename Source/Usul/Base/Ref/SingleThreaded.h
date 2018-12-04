
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for reference counting that is not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_SINGLE_THREADED_CLASS_H_
#define _USUL_BASE_SINGLE_THREADED_CLASS_H_

#include "Usul/Config/Config.h"


namespace Usul {
namespace Base {
namespace Ref {


class SingleThreaded
{
public:

  typedef unsigned long RefCount;


  void ref()
  {
    ++_refCount;
  }

  void unref ( bool allowDelete = true )
  {
    --_refCount;
    if ( ( 0 == _refCount ) && ( true == allowDelete ) )
    {
      delete this;
    }
  }

  RefCount refCount() const
  {
    return _refCount;
  }

protected:

  SingleThreaded() : _refCount ( 0 )
  {
  }

  SingleThreaded ( const SingleThreaded &r ) : _refCount ( 0 )
  {
  }

  SingleThreaded &operator = ( const SingleThreaded &r )
  {
    return *this;
  }

  virtual ~SingleThreaded()
  {
  }

private:

  RefCount _refCount;
};


} // namespace Ref
} // namespace Base
} // namespace Usul


#endif // _USUL_BASE_SINGLE_THREADED_CLASS_H_
