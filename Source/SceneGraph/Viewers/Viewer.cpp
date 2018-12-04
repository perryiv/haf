
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Viewers/Viewer.h"
#include "SceneGraph/Functors/Trackball.h"
#include "SceneGraph/Interfaces/IDrawMethodCreate.h"
#include "SceneGraph/Interfaces/ILookAtAll.h"
#include "SceneGraph/Interfaces/IResizeNotify.h"
#include "SceneGraph/Visitors/FrustumCull.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/System/Clock.h"

#include "boost/bind.hpp"
#include "boost/tuple/tuple_comparison.hpp"

#include <algorithm>

using namespace SceneGraph::Viewers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Viewer, Viewer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( IUnknown::RefPtr context ) : BaseClass(),
  _updatePipeline(),
  _cullPipeline(),
  _drawPipeline(),
  _drawLists ( new SceneGraph::Draw::Lists ),
  _eventListeners(),
  _renderQueue ( Usul::Jobs::Manager::instance()["render_queue"] ),
  _renderJobs(),
  _context ( context ),
  _scene(),
  _projection ( new Functors::Perspective ( 45, 1, 2, 10000 ) ),
  _navigation ( new Functors::MatrixReturn ( Matrix::getIdentity() ) ),
  _timeAllowed ( 1000 / 30 ),
  _viewport ( Viewport ( 0, 0, 100, 100 ) )
{
  // Reference this class and dereference without deleting.
  NoDeleteRefPtr me ( this );

  // Set default listeners.
  this->listenerAdd ( typeid ( SceneGraph::Events::Paint ), 
                      boost::bind ( &Viewer::_onPaint, this, _1 ) );
  this->listenerAdd ( typeid ( SceneGraph::Events::Resize ), 
                      boost::bind ( &Viewer::_onResize, this, _1 ) );

  // The left mouse button.
  Event::ButtonsDown leftMouse;
  leftMouse.insert ( 0 );
#if 0
  // Listener for left mouse press.
  {
    ListenerValue listener ( 0, false, Event::KeysDown(), true, leftMouse, 
      SceneGraph::Events::makeHandler ( boost::bind ( &Viewer::_onMousePress, this ) ) );
    this->listenerAdd ( typeid ( SceneGraph::Events::MousePress ), listener );
  }
#endif
  // Initialize default pipeline.
  Usul::Functions::noThrow ( boost::bind 
    ( &Viewer::_createDefaultPipelines, this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  Usul::Functions::noThrow ( boost::bind ( &Viewer::_destroy, this ), "1044348089" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel and wait for job.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void cancelAndWait ( Usul::Jobs::Queue::RefPtr queue, 
                              Usul::Jobs::BaseJob::RefPtr job )
  {
    if ( true == queue.valid() )
    {
      queue->remove ( job );
    }
    if ( true == job.valid() )
    {
      job->cancel();
    }
    if ( true == queue.valid() )
    {
      queue->wait ( job );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this object.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_destroy()
{
  // Cancel and wait for the render jobs.
  JobList jobs ( _renderJobs.fetchAndStore ( JobList() ) );
  std::for_each ( jobs.begin(), jobs.end(), 
    boost::bind ( Helper::cancelAndWait, _renderQueue, _1 ) );

  // Done with these.
  _updatePipeline.clear();
  _cullPipeline.clear();
  _drawPipeline.clear();
  _drawLists = DrawLists::RefPtr();
  _eventListeners.clear();
  _context = IContext::RefPtr();
  _scene = Node::RefPtr();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Viewer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case SceneGraph::Interfaces::INodeGet::IID:
    return static_cast < SceneGraph::Interfaces::INodeGet * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default pipelines.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_createDefaultPipelines()
{
  typedef SceneGraph::Interfaces::IDrawMethodCreate IDrawMethodCreate;
  typedef Usul::Plugins::Manager Manager;
  typedef SceneGraph::Visitors::FrustumCull FrustumCull;

  // Add the update visitor.
  {
    UpdateVisitor::RefPtr uv ( new UpdateVisitor );
    Guard guard ( _updatePipeline.mutex() );
    _updatePipeline.getReference().push_back ( uv );
  }

  // Add the cull visitor.
  {
    CullVisitor::RefPtr cv ( new FrustumCull );
    cv->drawListsSet ( _drawLists );
    Guard guard ( _cullPipeline.mutex() );
    _cullPipeline.getReference().push_back ( cv );
  }

  // Look for the plugin that can make a draw-method.
  Manager &pm ( Manager::instance() );
  IDrawMethodCreate::QueryPtr c ( pm.findFirst ( IDrawMethodCreate::IID ) );
  if ( true == c.valid() )
  {
    // Make the draw-method.
    DrawMethod::RefPtr dm ( c->createDrawMethod ( IContext::RefPtr ( _context ) ) );
    if ( true == dm.valid() )
    {
      dm->drawListsSet ( _drawLists );
      Guard guard ( _drawPipeline.mutex() );
      _drawPipeline.getReference().push_back ( dm );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::CullPipeline Viewer::pipelineCullGet() const
{
  return _cullPipeline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::pipelineCullSet ( const CullPipeline &p )
{
  _cullPipeline = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::DrawPipeline Viewer::pipelineDrawGet() const
{
  return _drawPipeline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::pipelineDrawSet ( const DrawPipeline &p )
{
  _drawPipeline = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::UpdatePipeline Viewer::pipelineUpdateGet() const
{
  return _updatePipeline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::pipelineUpdateSet ( const UpdatePipeline &p )
{
  _updatePipeline = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a copy of the listeners for the specified type.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ListenerSequence Viewer::_copyListeners ( const std::type_info &type ) const
{
  const char *name ( type.name() );
  const std::string key ( ( 0x0 != name ) ? name : std::string() );

  Guard guard ( _eventListeners.mutex() );
  const EventListeners &el ( _eventListeners.getReference() );
  EventListeners::const_iterator i ( el.find ( key ) );

  if ( el.end() == i )
  {
    return ListenerSequence();
  }
  else
  {
    return i->second;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a copy of the listeners for the specified type.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ListenerSequence Viewer::_copyListeners ( Event::RefPtr e ) const
{
  if ( true == e.valid() )
  {
    return this->_copyListeners ( typeid ( *( e.get() ) ) );
  }
  else
  {
    return ListenerSequence();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of an event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::notify ( Event::RefPtr e )
{
  // Copy the listeners for this event.
  ListenerSequence listeners ( this->_copyListeners ( e ) );

  // Loop through the listeners.
  for ( ListenerSequence::iterator i = listeners.begin(); i != listeners.end(); ++i )
  {
    // Get the listener.
    const ListenerValue &listener ( *i );

    // See if it passes the filter for keys.
    const bool keys ( ( true == listener.get<1>() ) ? 
      ( e->keysDown() == listener.get<2>() ) : true );

    // See if it passes the filter for buttons.
    const bool buttons ( ( true == listener.get<3>() ) ?
      ( e->buttonsDown() == listener.get<4>() ) : true );

    // If it passed both filters...
    if ( keys && buttons )
    {
      // Get the handler.
      Handler::RefPtr handler ( listener.get<5>() );

      // If the handler is valid then call it.
      if ( true == handler.valid() )
      {
        handler->handle ( e );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to sort the listeners.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct SortListeners
  {
    template < class T > bool operator () ( const T &left, const T &right ) const
    {
      return ( left.get<0>() < right.get<0>() );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerAdd 
  ( const std::type_info &type, const ListenerValue &listener )
{
  const char *name ( type.name() );
  const std::string key ( ( 0x0 != name ) ? name : std::string() );
  if ( true == key.empty() )
    return;

  if ( false == listener.get<5>().valid() )
    return;

  // Append the new listener.
  Guard guard ( _eventListeners.mutex() );
  ListenerSequence &listeners ( _eventListeners.getReference()[key] );
  listeners.push_back ( listener );

  // Keep the sequence sorted.
  std::sort ( listeners.begin(), listeners.end(), Helper::SortListeners() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a listener.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ListenerValue Viewer::listenerAdd 
  ( const std::type_info &type, Handler::RefPtr handler )
{
  ListenerValue listener 
    ( 0, false, Event::KeysDown(), false, Event::ButtonsDown(), handler );
  this->listenerAdd ( type, listener );
  return listener;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenersSet
  ( const std::type_info &type, const ListenerSequence &ls )
{
  const char *name ( type.name() );
  const std::string key ( ( 0x0 != name ) ? name : std::string() );
  if ( true == key.empty() )
    return;

  // Set the new listeners.
  Guard guard ( _eventListeners.mutex() );
  _eventListeners.getReference()[key] = ls;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the listeners.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ListenerSequence Viewer::listenersGet ( const std::type_info &type ) const
{
  typedef EventListeners::const_iterator Itr;

  const char *name ( type.name() );
  const std::string key ( ( 0x0 != name ) ? name : std::string() );
  if ( true == key.empty() )
    return ListenerSequence();

  // Return the listeners.
  Guard guard ( _eventListeners.mutex() );
  const EventListeners &el ( _eventListeners.getReference() );
  Itr i ( el.find ( key ) );
  return ( ( el.end() == i ) ? ListenerSequence() : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerRemove 
  ( const std::type_info &type, const ListenerValue &listener )
{
  const char *name ( type.name() );
  const std::string key ( ( 0x0 != name ) ? name : std::string() );

  if ( false == listener.get<5>().valid() )
    return;

  // Look for the sequence of listeners.
  Guard guard ( _eventListeners.mutex() );
  EventListeners &el ( _eventListeners.getReference() );
  EventListeners::iterator i ( el.find ( key ) );
  if ( el.end() == i )
    return;

  // Erase the listener.
  ListenerSequence &listeners ( i->second );
  listeners.erase ( std::find ( listeners.begin(), listeners.end(), listener ) );

  // Keep the sequence sorted while the reference is still valid.
  std::sort ( listeners.begin(), listeners.end(), Helper::SortListeners() );

  // If the sequence is empty then remove it.
  if ( true == listeners.empty() )
  {
    el.erase ( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerRemove 
  ( const std::type_info &type, Handler::RefPtr handler )
{
  ListenerValue listener 
    ( 0, false, Event::KeysDown(), false, Event::ButtonsDown(), handler );
  this->listenerRemove ( type, listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the render job.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Job::RefPtr Viewer::renderJobAdd()
{
  const Matrix n ( this->navigationMatrixGet() );
  const Matrix p ( this->projectionMatrixGet() );
  const ClockTics t ( this->timeAllowedGet() );
  return this->renderJobAdd ( n, p, t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the render job.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Job::RefPtr Viewer::renderJobAdd 
  ( const Matrix &nav, const Matrix &proj, ClockTics timeAllowed )
{
  typedef SceneGraph::Functors::MatrixGet MatrixGet;
  typedef SceneGraph::Functors::MatrixReturn MatrixReturn;
  MatrixGet::RefPtr n ( new MatrixReturn ( nav ) );
  MatrixGet::RefPtr p ( new MatrixReturn ( proj ) );
  IMatrixGet::QueryPtr in ( n );
  IMatrixGet::QueryPtr ip ( p );
  return this->renderJobAdd ( in, ip, timeAllowed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the render job.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Job::RefPtr Viewer::renderJobAdd 
  ( IMatrixGet::RefPtr nav, IMatrixGet::RefPtr proj, ClockTics timeAllowed )
{
  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );
  if ( false == queue.valid() )
    return Job::RefPtr();

  // Lock and get shortcut because we assign.
  typedef Usul::Atomic::Container < JobList > AtomicJobs;
  AtomicJobs::Guard guard ( _renderJobs.mutex() );
  JobList &jobs ( _renderJobs.getReference() );

#if 0
#ifdef _DEBUG
  std::cout << Usul::Strings::format ( 
    "Viewer::renderJobAdd(), timeAllowed = ", 
    timeAllowed, '\n' ) << std::flush;
#endif
#endif

  // Add a new job.
  Job::RefPtr job ( queue->add ( boost::bind 
    ( &Viewer::_render, this, nav, proj, timeAllowed, _1 ) ) );
  jobs.insert ( job );

  // Return this new job.
  return job;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call when the job finished.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_renderJobFinished ( Job::RefPtr job )
{
  // Lock and get shortcut because we modify.
  Guard guard ( _renderJobs.mutex() );
  JobList &jobs ( _renderJobs.getReference() );

  // Remove the first occurance of job.
  JobList::iterator i ( std::find ( jobs.begin(), jobs.end(), job ) );
  if ( jobs.end() != i )
    jobs.erase ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of existing render jobs.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::renderJobsCount() const
{
  return _renderJobs.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the render job.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderJobCancel ( Job::RefPtr job )
{
  // Handle bad input.
  if ( false == job.valid() )
    return;

  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );

  // Remove the job from the queue.
  if ( true == queue.valid() )
  {
    Usul::Functions::noThrow ( boost::bind 
      ( &Usul::Jobs::Queue::remove, queue, job ) );
  }

  // Remove and cancel the job.
  Usul::Functions::noThrow ( boost::bind 
    ( &Viewer::_renderJobFinished, this, job ) );
  Usul::Functions::noThrow ( boost::bind ( &Job::cancel, job ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the existing render jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderJobsCancel()
{
  // Get and reset the collection of jobs.
  JobList jobs ( _renderJobs.fetchAndStore ( JobList() ) );

  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );

  // Remove and cancel all our jobs.
  for ( JobList::iterator i = jobs.begin(); i != jobs.end(); ++i )
  {
    USUL_TRY_BLOCK
    {
      Job::RefPtr job ( *i );
      if ( true == job.valid() )
      {
        if ( true == queue.valid() )
        {
          queue->remove ( job );
        }
        job->cancel();
      }
    }
    USUL_DEFINE_CATCH_BLOCKS ( "4051191298" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return a good matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Viewer::Matrix getMatrix ( 
    SceneGraph::Interfaces::IMatrixGet::RefPtr a,
    SceneGraph::Interfaces::IMatrixGet::RefPtr b )
  {
    if ( true == a.valid() )
    {
      return a->matrixGet();
    }
    else if ( b.valid() )
    {
      return b->matrixGet();
    }
    else
    {
      return Viewer::Matrix::getIdentity();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to reset the pipeline.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class PipelineType >
  inline void resetPipeline ( PipelineType &p )
  {
    typedef typename PipelineType::iterator Iterator;
    typedef typename PipelineType::value_type VisitorPtr;

    for ( Iterator i = p.begin(); i != p.end(); ++i )
    {
      VisitorPtr v ( *i );
      if ( true == v.valid() )
      {
        v->reset();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to cull the scene.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void cull ( 
    Viewer::CullPipeline &cp, 
    SceneGraph::Nodes::Node::RefPtr scene,
    Usul::Jobs::BaseJob::RefPtr job,
    const Viewer::Matrix &navigation,
    const Viewer::Matrix &projection,
    const Viewer::Viewport &viewport,
    Viewer::ClockTics timeAllowed )
  {
    typedef Viewer::CullPipeline CullPipeline;
    typedef Viewer::CullVisitor CullVisitor;

    // Handle bad scene.
    if ( false == scene.valid() )
      return;

    // Loop through the cull-pipeline.
    for ( CullPipeline::iterator i = cp.begin(); i != cp.end(); ++i )
    {
      CullVisitor::RefPtr cv ( *i );
      if ( true == cv.valid() )
      {
        // Set the matrices and viewport.
        cv->navigationMatrixSet ( navigation );
        cv->projectionMatrixSet ( projection );
        cv->viewportSet ( viewport );

        // Set the job and make sure it's unset.
        Usul::Jobs::BaseJob::RefPtr nullJob ( 0x0 );
        Usul::Scope::Caller::RefPtr resetJob ( Usul::Scope::makeCaller
          ( boost::bind ( &CullVisitor::jobSet, cv, nullJob ) ) );
        cv->jobSet ( job );

        // Set the times.
        cv->timeAllowedSet ( timeAllowed );
        cv->startTimeSet ( Usul::System::Clock::milliseconds() );

        // Visit the scene.
        scene->accept ( *cv );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to draw the elements in the draw-list.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void draw ( 
    Viewer::DrawPipeline &dp, 
    Usul::Jobs::BaseJob::RefPtr job,
    const Viewer::Matrix &navigation,
    const Viewer::Matrix &projection,
    const Viewer::Viewport &viewport,
    Viewer::ClockTics timeAllowed )
  {
    typedef Viewer::DrawPipeline DrawPipeline;
    typedef Viewer::DrawMethod DrawMethod;

    // Loop through the draw-pipeline.
    for ( DrawPipeline::iterator i = dp.begin(); i != dp.end(); ++i )
    {
      DrawMethod::RefPtr dm ( *i );
      if ( true == dm.valid() )
      {
        // Set the projection matrix and viewport.
        dm->navigationMatrixSet ( navigation );
        dm->projectionMatrixSet ( projection );
        dm->viewportSet ( viewport );

        // Set the job and make sure it's unset.
        Usul::Jobs::BaseJob::RefPtr nullJob ( 0x0 );
        Usul::Scope::Caller::RefPtr resetJob ( Usul::Scope::makeCaller
          ( boost::bind ( &DrawMethod::jobSet, dm, nullJob ) ) );
        dm->jobSet ( job );

        // Set the times.
        dm->timeAllowedSet ( timeAllowed );
        dm->startTimeSet ( Usul::System::Clock::milliseconds() );

        // Draw the elements.
        dm->draw();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_render ( IMatrixGet::RefPtr n, 
                       IMatrixGet::RefPtr p, 
                       ClockTics timeAllowed, 
                       Usul::Jobs::BaseJob::RefPtr job )
{
  // Always remove this job from our collection.
  Usul::Scope::Caller::RefPtr removeJob ( Usul::Scope::makeCaller
    ( boost::bind ( &Viewer::_renderJobFinished, this, job ) ) );

  // Need this interface.
  IContext::QueryPtr context ( _context );
  if ( false == context.valid() )
    return;

  // Always swap buffers.
  Usul::Scope::Caller::RefPtr swapBuffers ( Usul::Scope::makeCaller 
    ( boost::bind ( &IContext::swapRenderingBuffers, context ) ) );

  // This thread needs to make the context current.
  context->makeContextCurrent();

  // Get the matrices.
  const Matrix nm ( Helper::getMatrix ( n, _navigation ) );
  const Matrix pm ( Helper::getMatrix ( p, _projection ) );

  // Get the viewport.
  const Viewport vp ( _viewport );

  // Clear the draw-lists.
  DrawLists::RefPtr dl ( _drawLists );
  if ( true == dl.valid() )
  {
    dl->clear();
  }

  // Copy the pipelines.
  CullPipeline cp ( _cullPipeline );
  DrawPipeline dp ( _drawPipeline );

  // Reset all members of the pipelines.
  Helper::resetPipeline ( cp );
  Helper::resetPipeline ( dp );

  // Eat timed-out exceptions when culling.
  try
  {
    ClockTics cullTimeAllowed ( timeAllowed / ( 2 * cp.size() ) );
    Helper::cull ( cp, _scene, job, nm, pm, vp, cullTimeAllowed );
  }
  catch ( const Usul::Exceptions::TimedOut & )
  {
  }

  // Eat timed-out exceptions when drawing.
  try
  {
    ClockTics drawTimeAllowed ( timeAllowed / ( 2 * dp.size() ) );
    Helper::draw ( dp, job, nm, pm, vp, drawTimeAllowed );
  }
  catch ( const Usul::Exceptions::TimedOut & )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navigationMatrixFunctorSet ( IUnknown::RefPtr m )
{
  _navigation = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Matrix Viewer::navigationMatrixGet() const
{
  IMatrixGet::RefPtr n ( _navigation );
  return ( ( true == n.valid() ) ? n->matrixGet() : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::projectionMatrixFunctorSet ( IUnknown::RefPtr m )
{
  _projection = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Matrix Viewer::projectionMatrixGet() const
{
  IMatrixGet::RefPtr p ( _projection );
  return ( ( true == p.valid() ) ? p->matrixGet() : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time allowed when rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::timeAllowedSet ( ClockTics t )
{
  _timeAllowed = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time allowed when rendering.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ClockTics Viewer::timeAllowedGet() const
{
  return _timeAllowed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::sceneSet ( Node::RefPtr n )
{
  _scene = n;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraph::Nodes::Node::RefPtr Viewer::sceneGet()
{
  return _scene;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the node.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Node::RefPtr Viewer::nodeGet()
{
  return this->sceneGet();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the viewer is resized.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onResize ( Event::RefPtr e )
{
  Events::Resize::CastPtr re ( e );
  if ( false == re.valid() )
    return;

  const double w ( re->width() );
  const double h ( re->height() );
  if ( ( w <= 0 ) || ( h <= 0 ) )
    return;

  typedef SceneGraph::Interfaces::IResizeNotify IResizeNotify;
  IMatrixGet::QueryPtr p ( _projection );
  IResizeNotify::QueryPtr rn ( p );
  if ( false == rn.valid() )
    return;

  rn->sizeSet ( w, h );
  _viewport = Viewport ( 0, 0, w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the viewer needs to be painted.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onPaint ( Event::RefPtr )
{
  if ( this->renderJobsCount() < 2 )
  {
    const Matrix n ( this->navigationMatrixGet() );
    const Matrix p ( this->projectionMatrixGet() );
    this->renderJobAdd ( n, p, 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the navigator to look at everything.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lookAtAll()
{
  // Get navigation functor.
  IMatrixGet::QueryPtr nav ( _navigation );

  // Get needed interface.
  typedef SceneGraph::Interfaces::ILookAtAll ILookAtAll;
  ILookAtAll::QueryPtr lla ( nav );
  if ( false == lla.valid() )
    return;

  // Set the matrix.
  lla->lookAtAll ( IUnknown::QueryPtr ( this ) );
}
