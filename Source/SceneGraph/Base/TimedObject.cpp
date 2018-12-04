
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Abstract base class for objects with a time budget.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Base/TimedObject.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/System/Clock.h"

#include <limits>

using namespace SceneGraph::Base;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimedObject::TimedObject() : BaseClass(),
  _timeAllowed ( 0 ),
  _startTime ( Usul::System::Clock::milliseconds() ),
  _job ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TimedObject::~TimedObject()
{
  USUL_TRY_BLOCK
  {
    _job = JobPtr();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "5106792010" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the start time.
//
///////////////////////////////////////////////////////////////////////////////

void TimedObject::startTimeSet ( ClockTics t )
{
  _startTime = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the start time.
//
///////////////////////////////////////////////////////////////////////////////

TimedObject::ClockTics TimedObject::startTimeGet() const
{
  return _startTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time allowed.
//
///////////////////////////////////////////////////////////////////////////////

void TimedObject::timeAllowedSet ( ClockTics allowed )
{
  _timeAllowed = allowed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time allowed.
//
///////////////////////////////////////////////////////////////////////////////

TimedObject::ClockTics TimedObject::timeAllowedGet() const
{
  return _timeAllowed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time remaining to render.
//
///////////////////////////////////////////////////////////////////////////////

TimedObject::ClockTics TimedObject::timeRemaining() const
{
  // If the time allowed is zero it means all the time needed.
  const ClockTics allowed ( this->timeAllowedGet() );
  if ( 0 == allowed )
  {
    // Return half the max because it's a big enough number to indicate 
    // "lots of time left" and it's also less likely to result in overflow.
    return ( std::numeric_limits<ClockTics>::max() / 2 );
  }

  // If we get to here then figure out how much time is left.
  const ClockTics now ( Usul::System::Clock::milliseconds() );
  const ClockTics start ( _startTime );
  const ClockTics used ( now - start );
  if ( used > allowed )
  {
    return 0;
  }
  else
  {
    const ClockTics remaining ( allowed - used );
    return remaining;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check if we should continue. Throw if we should not.
//
///////////////////////////////////////////////////////////////////////////////

void TimedObject::_checkContinue() const
{
  // Has the job been cancelled?
  const JobPtr job ( _job );
  if ( true == job.valid() )
  {
    if ( true == job->isCancelled() )
    {
      const std::string message ( "Message 2389039639: Timed object cancelled" );
      throw Usul::Exceptions::Cancelled ( message );
    }
  }

  // Are we out of time?
  if ( 0 == this->timeRemaining() )
  {
    const std::string message ( "Message 2951296128: Timed object out of time" );
    throw Usul::Exceptions::TimedOut ( message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the job.
//
///////////////////////////////////////////////////////////////////////////////

void TimedObject::jobSet ( JobPtr job )
{
  _job = job;
}
