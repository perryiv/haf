
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
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_UNIX_MUTEX_CLASS_H_
#define _USUL_THREADS_UNIX_MUTEX_CLASS_H_

#include "Usul/Config/Config.h"

#include <pthread.h>


namespace Usul {
namespace Threads {


class UnixMutex
{
public:

  // Construction/Destruction.
  UnixMutex();
  ~UnixMutex();

  // Lock the mutex.
  void            lock();

  // Unlock the mutex.
  void            unlock();

  // Try to lock the mutex. Will return true if the lock has been acquired.
  bool            trylock();

private:

  UnixMutex ( const UnixMutex & );             // No copying
  UnixMutex &operator = ( const UnixMutex & ); // No assignment

  pthread_mutex_t _mutex;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_UNIX_MUTEX_CLASS_H_
