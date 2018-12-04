
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class for Windows.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_WINDOWS_MUTEX_CLASS_H_
#define _USUL_THREADS_WINDOWS_MUTEX_CLASS_H_

#include "Usul/Config/Config.h"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>

// Declare TryEnterCriticalSection.
// See http://msdn.microsoft.com/en-us/library/ms686857(VS.85).aspx for function signature.
#if !defined(_WIN32_WINNT)
extern "C" BOOL WINAPI TryEnterCriticalSection(__inout  LPCRITICAL_SECTION lpCriticalSection);
#endif

namespace Usul {
namespace Threads {


class WindowsMutex
{
public:

  // Construction/Destruction.
  WindowsMutex();
  ~WindowsMutex();

  // Lock the mutex.
  void            lock();

  // Unlock the mutex.
  void            unlock();

  // Try to lock the mutex. Will return true if the lock has been acquired.
  bool            trylock();

private:

  WindowsMutex ( const WindowsMutex & );             // No copying
  WindowsMutex &operator = ( const WindowsMutex & ); // No assignment

  CRITICAL_SECTION _cs;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_WINDOWS_MUTEX_CLASS_H_
