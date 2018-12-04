
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Simple class with mutex interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_FAKE_MUTEX_CLASS_H_
#define _USUL_THREADS_FAKE_MUTEX_CLASS_H_

#include "Usul/Config/Config.h"


namespace Usul {
namespace Threads {


struct FakeMutex
{
  void lock()
  {
  }
  template < class T1, class T2 > void lock ( T1, T2 )
  {
  }
  void unlock()
  {
  }
  bool trylock()
  {
    return true;
  }
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_FAKE_MUTEX_CLASS_H_
