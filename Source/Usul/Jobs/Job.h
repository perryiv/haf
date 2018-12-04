
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_JOBS_JOB_H_
#define _USUL_JOBS_JOB_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Bool.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Export/Export.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Handle.h"
#include "Usul/Scope/Caller.h"

#include "boost/bind.hpp"
#include "boost/noncopyable.hpp"

#include <vector>


namespace Usul {
namespace Jobs {


///////////////////////////////////////////////////////////////////////////////
//
//  Base job class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT BaseJob : 
  public Usul::Base::Referenced, 
  public boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Jobs::Handle Handle;
  typedef Handle::ID ID;
  typedef Handle::Priority Priority;
  USUL_REFERENCED_CLASS ( BaseJob );

  // Set the flag for "cancelled" to true.
  void              cancel();

  // Execute this job.
  virtual void      execute();

  // Return this job's handle.
  Handle            handle() const;

  // Return this job's id.
  ID                id() const;

  // Have we been cancelled?
  bool              isCancelled() const;

  // Return this job's priority.
  Priority          priority() const;

  // Predicate class so that we can use RefPtr in std::set.
  struct Less
  {
    bool operator () ( const RefPtr &left, const RefPtr &right ) const
    {
      if ( false == left.valid() )
        return true;

      if ( false == right.valid() )
        return false;

      return ( left->handle() < right->handle() );
    }
  };

protected:

  BaseJob ( Priority p = 0 );
  virtual ~BaseJob();

private:

  const Handle _handle;
  Usul::Atomic::Bool _cancelled;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete job class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class SimpleJob : public BaseJob
{
public:

  typedef BaseJob BaseClass;
  USUL_REFERENCED_CLASS ( SimpleJob );

  SimpleJob ( Function f, Priority p = 0 ) : BaseClass ( p ), _function ( f ){}

  virtual void execute()
  {
    this->ref();
    Usul::Scope::Caller::RefPtr unref ( Usul::Scope::makeCaller 
      ( boost::bind ( &SimpleJob::unref, this, false ) ) );
    {
      _function ( BaseClass::RefPtr ( this ) );
    }
  }

protected:

  virtual ~SimpleJob(){}

private:

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete job class that executes a pair of jobs.
//
///////////////////////////////////////////////////////////////////////////////

class Pair : public BaseJob
{
public:

  typedef BaseJob BaseClass;
  USUL_REFERENCED_CLASS ( Pair );

  Pair ( BaseJob::RefPtr job1, BaseJob::RefPtr job2, Priority p = 0 ) : 
    BaseClass ( p ), _job1 ( job1 ), _job2 ( job2 ){}

  virtual void execute()
  {
    if ( true == _job1.valid() )
    {
      _job1->execute();
    }
    if ( true == _job2.valid() )
    {
      _job2->execute();
    }
  }

protected:

  virtual ~Pair()
  {
    _job1 = BaseJob::RefPtr ( 0x0 );
    _job2 = BaseJob::RefPtr ( 0x0 );
  }

private:

  BaseJob::RefPtr _job1;
  BaseJob::RefPtr _job2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete job class that executes a sequence of jobs.
//
///////////////////////////////////////////////////////////////////////////////

class Sequence : public BaseJob
{
public:

  typedef BaseJob BaseClass;
  typedef std::vector < BaseJob::RefPtr > JobList;
  USUL_REFERENCED_CLASS ( Sequence );

  Sequence ( const JobList &j, Priority p = 0 ) : BaseClass ( p ), _jobs ( j ){}

  virtual void execute()
  {
    for ( JobList::iterator i = _jobs.begin(); i != _jobs.end(); ++i )
    {
      JobList::value_type job ( *i );
      if ( true == job.valid() )
      {
        job->execute();
      }
    }
  }

protected:

  virtual ~Sequence()
  {
    _jobs.clear();
  }

private:

  JobList _jobs;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for making jobs.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > inline BaseJob::RefPtr makeJob ( BaseJob::Priority p, F f )
{
  return BaseJob::RefPtr ( new SimpleJob < F > ( f, p ) );
}
template < class F1, class F2 >
inline BaseJob::RefPtr makeJob ( BaseJob::Priority p, F1 f1, F2 f2 )
{
  return BaseJob::RefPtr ( new Pair ( Usul::Jobs::makeJob ( p, f1 ), Usul::Jobs::makeJob ( p, f2 ) ) );
}
template < class F1, class F2, class F3 >
inline BaseJob::RefPtr makeJob ( BaseJob::Priority p, F1 f1, F2 f2, F3 f3 )
{
  BaseJob::RefPtr first ( Usul::Jobs::makeJob ( p, f1, f2 ) );
  return BaseJob::RefPtr ( new Pair ( first, Usul::Jobs::makeJob ( p, f3 ) ) );
}
template < class F1, class F2, class F3, class F4 >
inline BaseJob::RefPtr makeJob ( BaseJob::Priority p, F1 f1, F2 f2, F3 f3, F4 f4 )
{
  BaseJob::RefPtr first ( Usul::Jobs::makeJob ( p, f1, f2, f3 ) );
  return BaseJob::RefPtr ( new Pair ( first, Usul::Jobs::makeJob ( p, f4 ) ) );
}
template < class F1, class F2, class F3, class F4, class F5 >
inline BaseJob::RefPtr makeJob ( BaseJob::Priority p, F1 f1, F2 f2, F3 f3, F4 f4, F5 f5 )
{
  BaseJob::RefPtr first ( Usul::Jobs::makeJob ( p, f1, f2, f3, f4 ) );
  return BaseJob::RefPtr ( new Pair ( first, Usul::Jobs::makeJob ( p, f5 ) ) );
}


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_JOB_H_
