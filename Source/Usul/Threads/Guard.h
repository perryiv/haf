
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A scoped locking class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_GUARD_CLASS_H_
#define _USUL_THREADS_GUARD_CLASS_H_

#include "Usul/Config/Config.h"


namespace Usul {
namespace Threads {


template < class MutexType_ > class Guard
{
public:

  typedef MutexType_ MutexType;

  // Constructor. Locks the mutex.
  Guard ( MutexType &m ) : _mutex ( m )
  {
    _mutex.lock();
  }

  // Constructor. Locks the mutex.
  template < class ObjectType > Guard ( ObjectType *object ) : _mutex ( object->mutex() )
  {
    _mutex.lock();
  }

  // Destructor. Unlocks the mutex.
  ~Guard()
  {
    _mutex.unlock();
  }

protected:

  MutexType &_mutex;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_GUARD_CLASS_H_
