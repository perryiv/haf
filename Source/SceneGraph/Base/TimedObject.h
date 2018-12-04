
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

#ifndef _SCENE_GRAPH_TIMED_OBJECT_CLASS_H_
#define _SCENE_GRAPH_TIMED_OBJECT_CLASS_H_

#include "SceneGraph/Base/Object.h"

#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Types/Types.h"


namespace SceneGraph {
namespace Base {


class SCENE_GRAPH_EXPORT TimedObject : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( TimedObject, SceneGraph::Base::Object );

  typedef Usul::Types::UInt64 ClockTics;
  typedef Usul::Jobs::BaseJob::RefPtr JobPtr;

  // Set the job.
  void                    jobSet ( JobPtr );

  // Set/get the start time.
  ClockTics               startTimeGet() const;
  void                    startTimeSet ( ClockTics );

  // Set/get the time allowed.
  ClockTics               timeAllowedGet() const;
  void                    timeAllowedSet ( ClockTics );

  // Get the time remaining to render.
  ClockTics               timeRemaining() const;

protected:

  TimedObject();
  virtual ~TimedObject();

  void                    _checkContinue() const;

private:

  Usul::Atomic::Integer < ClockTics > _timeAllowed;
  Usul::Atomic::Integer < ClockTics > _startTime;
  Usul::Atomic::Object < JobPtr > _job;
};


} // namespace Base
} // namespace SceneGraph


#endif // _SCENE_GRAPH_TIMED_OBJECT_CLASS_H_
