
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A job-queue. Can execute in a thread-pool or calling thread.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/Caller.h"
#include "Usul/System/Clock.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/ThreadId.h"

#include "boost/bind.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/thread_time.hpp"

#include <iostream>
#include <stdexcept>

using namespace Usul::Jobs;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Queue::Queue ( unsigned int numThreads ) : 
  _threads(),
  _started ( false ),
  _cancelled ( false ),
  _sleep ( 10 ),
  _queued(),
  _running()
{
  if ( numThreads > 0 )
  {
    _threads.getReference().resize ( numThreads );
  }

  Usul::Registry::Database &reg ( Usul::Registry::Database::instance() );
  Usul::Registry::Node &section ( reg["main_window"]["worker_thread"]["sleep_duration"] );
  _sleep = section.get<unsigned long> ( _sleep, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Queue::~Queue()
{
  USUL_TRY_BLOCK
  {
    // Clear the queued jobs.
    _queued.clear();

    // Cancel the running threads.
    _cancelled = true;

    // Copy the threads and clear our member.
    ThreadList threads;
    {
      Threads::Guard guard ( _threads.mutex() );
      threads = _threads;
      _threads.clear();
    }

    // Wait for all the threads.
    for ( ThreadList::iterator i = threads.begin(); i != threads.end(); ++i )
    {
      ThreadPtr t ( *i );

      // If the threads were never started then they will still be null.
      if ( 0x0 != t.get() )
      {
        t->join();
      }
    }

    // It's possible for the threads to finish and some jobs to still be in the
    // running set. This is rare, but causes a structured exception upon exit.
    while ( this->numRunning() > 0 )
    {
      Usul::System::Sleep::milliseconds ( 10 );
    }

    // When we get to here there should not be any running jobs.
    USUL_ASSERT ( true == _running.empty() );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2639042759" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::add ( BaseJob::RefPtr job )
{
  // Handle bad input.
  if ( false == job.valid() )
    return;

  // Insert the new job.
  {
    AtomicJobSet::Guard guard ( _queued.mutex() );
    _queued.getReference().insert ( job );
  }

  // Make sure the threads are started.
  this->_startThreads();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the queue. This has no effect on jobs already running.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::clear()
{
  AtomicJobSet::Guard guard ( _queued.mutex() );
  JobSet &queue ( _queued.getReference() );
  queue.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the job.
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::remove ( BaseJob::RefPtr job )
{
  AtomicJobSet::Guard guard ( _queued.mutex() );
  const unsigned int num ( _queued.getReference().erase ( job ) );
  return ( num > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the threads.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_startThreads()
{
  // If we have not been started...
  if ( false == _started )
  {
    Threads::Guard guard ( _threads.mutex() );
    ThreadList &threads ( _threads.getReference() );

    // Now that we have the threads locked, check the flag again. 
    // This handles two or more threads ebtering this function at the same time.
    if ( false == _started )
    {
      for ( ThreadList::iterator i = threads.begin(); i != threads.end(); ++i )
      {
	ThreadPtr &thread ( *i );
	thread = ThreadPtr ( new boost::thread ( boost::bind ( &Queue::_runThread, this ) ) );
      }

      // Set the flag for quick checking.
      _started = true;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_runThread()
{
  while ( false == _cancelled )
  {
    Usul::Functions::noThrow ( boost::bind ( &Queue::_executeNextJob, this ), "2797080869" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the next job or sleep if there are no jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_executeNextJob()
{
  // This will return false if there are no jobs.
  if ( false == this->executeNextJob() )
  {
    boost::this_thread::sleep ( boost::posix_time::milliseconds ( _sleep ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the next job. Return false if there are no jobs.
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::executeNextJob()
{
  // Get the next job.
  BaseJob::RefPtr job ( this->_getNextJob() );

  // If we have a job...
  if ( true == job.valid() )
  {
    // Always remove it from the running collection.
    Usul::Scope::Caller::RefPtr remove ( Usul::Scope::makeCaller 
      ( boost::bind ( &Queue::_runningJobRemove, this, job ) ) );

    // Execute the job.
    this->_executeJob ( job );

    // If we get to here then we successfully executed the job.
    return true;
  }

  // No job.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next job.
//
///////////////////////////////////////////////////////////////////////////////

BaseJob::RefPtr Queue::_getNextJob()
{
  // Initialize the next job.
  BaseJob::RefPtr next ( 0x0 );

  // Local scope to minimize the time we lock.
  {
    // Lock queue and get shortcut.
    AtomicJobSet::Guard guard ( _queued.mutex() );
    AtomicJobSet::ValueType &q ( _queued.getReference() );

    // If there are jobs...
    if ( false == q.empty() )
    {
      // The next job will always be at the beginning of the map.
      AtomicJobSet::ValueType::iterator i ( q.begin() );
      next = *i;

      // Erase the job from the queue.
      q.erase ( i );
    }
  }

  // If we have a job...
  if ( true == next.valid() )
  {
    // Add job to running collection.
    this->_runningJobAdd ( next );
  }

  // Return the next job, which may be null.
  return next;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the job.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_executeJob ( BaseJob::RefPtr job )
{
  if ( false == job.valid() )
    return;

  USUL_TRY_BLOCK
  {
    job->execute();
  }

  catch ( const Usul::Exceptions::Cancelled & )
  {
    return;
  }

  catch ( const Usul::Exceptions::Restart & )
  {
    this->add ( job );
    return;
  }

  catch ( const Usul::Exceptions::Error &e )
  {
    std::cout << Usul::Strings::format ( 
      "Error ", e.id(), " caught while running job ", 
      job->id(), ", thread ", Usul::Threads::currentThreadId(), ", ", 
      e.message(), '\n' ) << std::flush;
  }

  catch ( const std::exception &e )
  {
    std::cout << Usul::Strings::format ( 
      "Error 8569575400: standard exception caught while running job ", 
      job->id(), ", thread ", Usul::Threads::currentThreadId(), ", ", 
      e.what(), '\n' ) << std::flush;
  }

  catch ( ... )
  {
    std::cout << Usul::Strings::format ( 
      "Error 4268555760: unknown exception caught while running thread ", 
      job->id(), ", thread ", Usul::Threads::currentThreadId(), 
      '\n' ) << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of jobs.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Queue::size() const
{
  return _queued.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the queue empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::empty() const
{
  return _queued.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sleep duration.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::sleepDurationSet ( unsigned long milliseconds )
{
  _sleep = milliseconds;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the sleep duration.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Queue::sleepDurationGet() const
{
  return _sleep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for the job to finish.
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::wait ( BaseJob::RefPtr job, unsigned long s, Usul::Types::UInt64 timeout )
{
  // Capture the start time.
  const Usul::Types::UInt64 start ( Usul::System::Clock::milliseconds() );

  // Loop until the job is done or we're out of time.
  while ( true )
  {
    // If the job is not running then we're done.
    if ( false == this->isRunning ( job ) )
    {
      return true;
    }

    // See if we've taken too long.
    if ( timeout > 0 )
    {
      const Usul::Types::UInt64 now ( Usul::System::Clock::milliseconds() );
      if ( ( now - start ) >= timeout )
      {
        return false;
      }
    }

    // Sleep for a while.
    Usul::System::Sleep::milliseconds ( s );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the job running?
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::isRunning ( BaseJob::RefPtr job ) const
{
  // Lock running collection and get shortcut.
  AtomicJobSet::Guard guard ( _running.mutex() );
  const AtomicJobSet::ValueType &r ( _running.getReference() );

  // Look for the job.
  AtomicJobSet::ValueType::const_iterator i ( r.find ( job ) );
  return ( r.end() != i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the job to the running collection.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_runningJobAdd ( BaseJob::RefPtr job )
{
  if ( true == job.valid() )
  {
    AtomicJobSet::Guard guard ( _running.mutex() );
    _running.getReference().insert ( job );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the job from the running collection.
//
///////////////////////////////////////////////////////////////////////////////

void Queue::_runningJobRemove ( BaseJob::RefPtr job )
{
  AtomicJobSet::Guard guard ( _running.mutex() );
  _running.getReference().erase ( job );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of running jobs.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Queue::numRunning() const
{
  AtomicJobSet::Guard guard ( _running.mutex() );
  const unsigned int num ( _running.getReference().size() );
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of threads.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Queue::numThreads() const
{
  Threads::Guard guard ( _threads.mutex() );
  const unsigned int num ( _threads.getReference().size() );
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if a higher-priority job is waiting.
//
///////////////////////////////////////////////////////////////////////////////

bool Queue::isHigherPriorityJobWaiting ( BaseJob::Priority priority ) const
{
  // Lock the queue.
  Threads::Guard guard ( _queued.mutex() );

  // Get a shortcut.
  const JobSet &queued ( _queued.getReference() );

  // If it's not empty...
  if ( false == queued.empty() )
  {
    // Get the the next job.
    const BaseJob::RefPtr &job ( *( queued.begin() ) );

    // Make sure it is valid...
    if ( true == job.valid() )
    {
      // A more negative priority is "higher".
      if ( job->priority() < priority )
      {
	// The pending job is higher priority.
	return true;
      }
    }
  }

  // If we get to here then the pending job is not higher priority.
  return false;
}
