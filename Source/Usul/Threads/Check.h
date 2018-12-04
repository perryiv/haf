
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Simple class that makes sure it is called from the correct thread.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_CHECK_THREAD_CLASS_H_
#define _USUL_THREADS_CHECK_THREAD_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Errors/Assert.h"

#include <stdexcept>


namespace Usul {
namespace Threads {


class Check
{
public:

  Check ( unsigned long id = 0 ) : _id ( ( 0 == id ) ? Usul::Threads::currentThreadId() : id )
  {
  }

  Check ( const Check &c ) : _id ( c._id )
  {
  }

  bool isSameThread() const
  {
    return ( Usul::Threads::currentThreadId() == _id );
  }

  void throwIfSameThread() const
  {
    this->_throwIf ( true == this->isSameThread() );
  }

  void throwIfDifferentThread() const
  {
    this->_throwIf ( false == this->isSameThread() );
  }

protected:

  void _throwIf ( bool state, const char *error = 0x0 ) const
  {
    if ( true == state )
    {
      USUL_ASSERT ( false );
      error = ( ( 0x0 == error ) ? "1876886060" : error );
      throw std::runtime_error ( Usul::Strings::format ( 
        "Error ", error, ": correct thread is ", _id, 
        " but calling thread is ", Usul::Threads::currentThreadId() ) );
    }

  }

private:

  Check &operator = ( const Check & );

  const unsigned long _id;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_CHECK_THREAD_CLASS_H_
