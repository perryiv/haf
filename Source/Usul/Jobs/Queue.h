
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

#ifndef _USUL_JOBS_QUEUE_H_
#define _USUL_JOBS_QUEUE_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Atomic/Bool.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Types/Types.h"

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

#include <set>
#include <vector>

namespace boost { class thread; }


namespace Usul {
namespace Jobs {


///////////////////////////////////////////////////////////////////////////////
//
//  Class that executes jobs either in calling thread or thread-pool.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Queue : 
  public Usul::Base::Referenced, 
  public boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef boost::shared_ptr < boost::thread > ThreadPtr;
  typedef std::vector < ThreadPtr > ThreadList;
  typedef Usul::Atomic::Container < ThreadList > Threads;
  typedef BaseJob::Priority Priority;
  typedef std::set < BaseJob::RefPtr, BaseJob::Less > JobSet;
  typedef Usul::Atomic::Container < JobSet > AtomicJobSet;

  USUL_REFERENCED_CLASS ( Queue );

  // Pass zero for use in calling thread.
  Queue ( unsigned int numThreads );

  // Add the job.
  template < class Function > 
  BaseJob::RefPtr       add ( Function f, Priority p = 0 );
  void                  add ( BaseJob::RefPtr );

  // Clear the queue. This has no effect on jobs already running.
  void                  clear();

  // Is the queue empty?
  bool                  empty() const;

  // Execute the next job if there is one. Call this when using in the 
  // same thread. Calling this is not needed if the queue was instantiated 
  // with numThreads > 0. Returns false if there are no jobs queued.
  bool                  executeNextJob();

  // Is there a higher-priority job is waiting?
  bool                  isHigherPriorityJobWaiting ( BaseJob::Priority priority ) const;


  // Is the job running?
  bool                  isRunning ( BaseJob::RefPtr ) const;

  // Return the number of running jobs.
  unsigned int          numRunning() const;

  // Return the number of threads.
  unsigned int          numThreads() const;

  // Remove the job from the queue. This does not cancel a running job.
  bool                  remove ( BaseJob::RefPtr );

  // Get the running jobs.
  template < class SetType >
  void                  running ( SetType & ) const;

  // Return size of the queue.
  unsigned int          size() const;

  // Set/get the sleep duration. The threads sleep and then look for a job.
  void                  sleepDurationSet ( unsigned long milliseconds );
  unsigned long         sleepDurationGet() const;

  // Get the queued jobs.
  template < class SetType >
  void                  queued ( SetType & ) const;

  // Wait for the job to finish. Returns false if it timed out.
  // Returns true if it found and waited for the job to finish.
  // Also returns true if it did not find the job.
  bool                  wait ( BaseJob::RefPtr, unsigned long sleepDuration = 100, Usul::Types::UInt64 timeout = 0 );

protected:

  virtual ~Queue();

  void                  _executeJob ( BaseJob::RefPtr );

  void                  _executeNextJob();

  BaseJob::RefPtr       _getNextJob();

  void                  _runningJobAdd ( BaseJob::RefPtr );
  void                  _runningJobRemove ( BaseJob::RefPtr );

  void                  _runThread();

  void                  _startThreads();

private:

  Threads _threads;
  Usul::Atomic::Bool _started;
  Usul::Atomic::Bool _cancelled;
  Usul::Atomic::Integer < unsigned long > _sleep;
  AtomicJobSet _queued;
  AtomicJobSet _running;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Add the job.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > BaseJob::RefPtr Queue::add ( F f, Priority p )
{
  BaseJob::RefPtr job ( Usul::Jobs::makeJob<F> ( p, f ) );
  this->add ( job );
  return job;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the running jobs.
//
///////////////////////////////////////////////////////////////////////////////

template < class SetType > void Queue::running ( SetType &s ) const
{
  AtomicJobSet::Guard guard ( _running.mutex() );
  const JobSet &jobs ( _running.getReference() );
  s.insert ( jobs.begin(), jobs.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the queued jobs.
//
///////////////////////////////////////////////////////////////////////////////

template < class SetType > void Queue::queued ( SetType &s ) const
{
  AtomicJobSet::Guard guard ( _queued.mutex() );
  const JobSet &jobs ( _queued.getReference() );
  s.insert ( jobs.begin(), jobs.end() );
}


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_QUEUE_H_
