
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread-safe convenience functions and macros.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_
#define _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_

#include "Usul/Threads/Guard.h"


namespace Usul {
namespace Threads {
namespace Safe {


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to block while getting the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType, class T > T get ( MutexType &mutex, const T &t, bool copyFirst = false )
{
  Usul::Threads::Guard<MutexType> guard ( mutex );
  return ( ( true == copyFirst ) ? T ( t ) : t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to block while setting the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType, class T1, class T2 > void set ( MutexType &mutex, const T1 &from, T2 &to )
{
  Usul::Threads::Guard<MutexType> guard ( mutex );
  to = from;
}


} // namespace Safe
} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_
