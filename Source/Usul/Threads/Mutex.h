
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_MUTEX_CLASS_H_
#define _USUL_THREADS_MUTEX_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#ifdef _WIN32
# define USUL_WINDOWS
#endif

#ifdef _WIN64
# define USUL_WINDOWS
#endif

#ifdef USUL_WINDOWS
#include "Usul/Threads/WindowsMutex.h"
#else
#include "Usul/Threads/UnixMutex.h"
#endif


namespace Usul {
namespace Threads {


#ifdef USUL_WINDOWS
class USUL_EXPORT Mutex : public WindowsMutex
#else
class USUL_EXPORT Mutex : public UnixMutex
#endif
{
public:

  #ifdef USUL_WINDOWS
  typedef WindowsMutex BaseClass;
  #else
  typedef UnixMutex BaseClass;
  #endif

  // Construction/Destruction.
  Mutex();
  ~Mutex();

  // Lock the mutex.
  void            lock() { BaseClass::lock(); }
  void            lock ( Usul::Types::UInt64 timeout, unsigned long millisecondPause = 500 );

  // Unlock the mutex.
  void            unlock() { BaseClass::unlock(); }

  // Try to lock the mutex. Will return true if the lock has been acquired.
  bool            trylock() { return BaseClass::trylock(); }

private:

  Mutex ( const Mutex & );             // No copying
  Mutex &operator = ( const Mutex & ); // No assignment
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_MUTEX_CLASS_H_
