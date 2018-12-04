
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class,
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Mutex.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Clock.h"
#include "Usul/System/Sleep.h"

#include <stdexcept>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::Mutex() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::~Mutex()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::lock ( Usul::Types::UInt64 timeout, unsigned long pause )
{
  // Get the current time.
  const Usul::Types::UInt64 start ( Usul::System::Clock::milliseconds() );

  // Try to acquire the lock...
  while ( false == this->trylock() )
  {
    // Calculate how long we have been trying.
    const Usul::Types::UInt64 now ( Usul::System::Clock::milliseconds() );
    const Usul::Types::UInt64 duration ( now - start );

    // If the time is greater than the timeout, throw an exception.
    if ( duration > timeout )
    {
      throw Usul::Exceptions::TimedOut ( Usul::Strings::format 
        ( "Warning 1373313702: Could not acquire lock within ", timeout, " milliseconds." ) );
    }

    // Try again in "pause" milliseconds.
    Usul::System::Sleep::milliseconds ( pause );
  }
}
