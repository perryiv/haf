
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_JOBS_H_
#define _SCENE_GRAPH_JOBS_H_

#include "Usul/Jobs/Job.h"
#include "Usul/Math/Matrix44.h"


namespace SceneGraph {
namespace Jobs {


///////////////////////////////////////////////////////////////////////////////
//
//  Render job class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class RenderJob : public Usul::Jobs::BaseJob
{
public:

  typedef Usul::Jobs::BaseJob BaseClass;
  typedef Usul::Math::Matrix44d Matrix;
  USUL_REFERENCED_CLASS ( RenderJob );

  RenderJob ( Function f, const Matrix &m, BaseClass::Priority p ) : BaseClass ( p ),
    _matrix ( m ),
    _function ( f )
  {
  }

  virtual void execute()
  {
    RenderJob::NoDeleteRefPtr me ( this );
    {
      _function ( _matrix, BaseClass::RefPtr ( this ) );
    }
  }

protected:

  virtual ~RenderJob(){}

private:

  const Matrix _matrix;
  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for making jobs.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > inline Usul::Jobs::BaseJob::RefPtr makeJob ( F f, Usul::Jobs::BaseJob::Priority p = 0 )
{
  return Usul::Jobs::BaseJob::RefPtr ( new RenderJob < F > ( f, p ) );
}


} // namespace Jobs
} // namespace SceneGraph


#endif // _SCENE_GRAPH_JOBS_H_
