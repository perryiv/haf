
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Function for getting the thread-id. This is inline because I do not 
//  want Usul to depend on pthreads.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_ID_H_
#define _USUL_THREADS_THREAD_ID_H_

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include "windows.h"
#else
# include "Usul/Cast/Cast.h"
# include <pthread.h>
#endif


namespace Usul {
namespace Threads {


inline unsigned long currentThreadId()
{
  #ifdef _MSC_VER
    return ::GetCurrentThreadId();
  #else
    return Usul::Cast::unsafe<unsigned long> ( ::pthread_self() );
  #endif
}


} // namespace Threads
} // namespace Usul

#endif // _USUL_THREADS_THREAD_ID_H_
