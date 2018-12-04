
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

#ifndef _SCENE_GRAPH_VIEWER_CLASS_H_
#define _SCENE_GRAPH_VIEWER_CLASS_H_

#include "SceneGraph/Base/Object.h"
#include "SceneGraph/Draw/Lists.h"
#include "SceneGraph/Draw/Method.h"
#include "SceneGraph/Events/Handlers.h"
#include "SceneGraph/Interfaces/IContext.h"
#include "SceneGraph/Interfaces/IMatrixGet.h"
#include "SceneGraph/Interfaces/INodeGet.h"
#include "SceneGraph/Nodes/Node.h"
#include "SceneGraph/Visitors/CullVisitor.h"
#include "SceneGraph/Visitors/UpdateVisitor.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Commands/Commands.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector4.h"

#include "boost/tuple/tuple.hpp"

#include <map>
#include <string>
#include <typeinfo>
#include <vector>


namespace SceneGraph {
namespace Viewers {


class SCENE_GRAPH_EXPORT Viewer : 
  public SceneGraph::Base::Object,
  public SceneGraph::Interfaces::INodeGet
{
public:

  SCENE_GRAPH_OBJECT ( Viewer, SceneGraph::Base::Object );
  typedef SceneGraph::Visitors::UpdateVisitor UpdateVisitor;
  typedef SceneGraph::Visitors::CullVisitor CullVisitor;
  typedef SceneGraph::Draw::Method DrawMethod;
  typedef SceneGraph::Draw::Lists DrawLists;
  typedef std::vector < UpdateVisitor::RefPtr > UpdatePipeline;
  typedef std::vector < CullVisitor::RefPtr > CullPipeline;
  typedef std::vector < DrawMethod::RefPtr > DrawPipeline;
  typedef SceneGraph::Nodes::Node Node;
  typedef SceneGraph::Events::Event Event;
  typedef SceneGraph::Events::BaseHandler Handler;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef Usul::Jobs::Queue JobQueue;
  typedef Usul::Jobs::BaseJob Job;
  typedef Usul::Jobs::Queue::JobSet JobList;
  typedef Usul::Types::UInt64 ClockTics;
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef SceneGraph::Interfaces::IContext IContext;
  typedef SceneGraph::Interfaces::IMatrixGet IMatrixGet;
  typedef boost::tuples::tuple
  <
    int,                // Sorting priority
    bool,               // Do we care about keyboard state?
    Event::KeysDown,    // Keyboard state
    bool,               // Do we care about mouse button state?
    Event::ButtonsDown, // Mouse button state
    Handler::RefPtr     // The function called.
  >
  ListenerValue;
  typedef std::vector < ListenerValue > ListenerSequence;
  typedef std::map < std::string, ListenerSequence > EventListeners;
  typedef Usul::Math::Vec4d Viewport;

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor
  Viewer ( IUnknown::RefPtr context );

  // Add and remove listeners.
  template < class Function >
  ListenerValue               listenerAdd    ( const std::type_info &t, Function f ) { return this->listenerAdd ( t, SceneGraph::Events::makeHandler ( f ) ); }
  ListenerValue               listenerAdd    ( const std::type_info &t, Handler::RefPtr );
  void                        listenerAdd    ( const std::type_info &t, const ListenerValue & );
  void                        listenerRemove ( const std::type_info &t, Handler::RefPtr );
  void                        listenerRemove ( const std::type_info &t, const ListenerValue & );

  // Get/set/clear all listeners for a given event type.
  void                        listenersClear ( const std::type_info &t ) { this->listenersSet ( t, ListenerSequence() ); }
  void                        listenersSet   ( const std::type_info &, const ListenerSequence & );
  ListenerSequence            listenersGet   ( const std::type_info & ) const;

  // Tell the navigator to look at everything.
  virtual void                lookAtAll();

  // Get/set the navigation matrix.
  Matrix                      navigationMatrixGet() const;
  void                        navigationMatrixFunctorSet ( IUnknown::RefPtr );

  // SceneGraph::Interfaces::INodeGet
  virtual Node::RefPtr        nodeGet();

  // Notification of an event.
  void                        notify ( Event::RefPtr );

  // Get/set the pipelines.
  CullPipeline                pipelineCullGet() const;
  void                        pipelineCullSet ( const CullPipeline & );
  DrawPipeline                pipelineDrawGet() const;
  void                        pipelineDrawSet ( const DrawPipeline & );
  UpdatePipeline              pipelineUpdateGet() const;
  void                        pipelineUpdateSet ( const UpdatePipeline & );

  // Get/set the projection matrix.
  Matrix                      projectionMatrixGet() const;
  void                        projectionMatrixFunctorSet ( IUnknown::RefPtr );

  // Functions for working with render jobs.
  Job::RefPtr                 renderJobAdd();
  Job::RefPtr                 renderJobAdd ( const Matrix &navigation, const Matrix &projection, ClockTics timeAllowed );
  Job::RefPtr                 renderJobAdd ( IMatrixGet::RefPtr navigation, IMatrixGet::RefPtr projection, ClockTics timeAllowed );
  void                        renderJobCancel ( Job::RefPtr );
  void                        renderJobsCancel();
  unsigned int                renderJobsCount() const;

  // Get/set the scene.
  Node::RefPtr                sceneGet();
  void                        sceneSet ( Node::RefPtr );

  // Get/set the default time allowed when rendering.
  ClockTics                   timeAllowedGet() const;
  void                        timeAllowedSet ( ClockTics );

protected:

  virtual ~Viewer();

  ListenerSequence            _copyListeners ( const std::type_info & ) const;
  ListenerSequence            _copyListeners ( Event::RefPtr ) const;
  void                        _createDefaultPipelines();

  void                        _onPaint  ( Event::RefPtr );
  void                        _onResize ( Event::RefPtr );

  void                        _render ( IMatrixGet::RefPtr navigation, IMatrixGet::RefPtr projection, ClockTics timeAllowed, Job::RefPtr job );
  void                        _renderJobFinished ( Job::RefPtr job );

private:

  void                        _destroy();

  Usul::Atomic::Container < UpdatePipeline > _updatePipeline;
  Usul::Atomic::Container < CullPipeline > _cullPipeline;
  Usul::Atomic::Container < DrawPipeline > _drawPipeline;
  Usul::Atomic::Object < DrawLists::RefPtr > _drawLists;
  Usul::Atomic::Container < EventListeners > _eventListeners;
  Usul::Atomic::Object < JobQueue::RefPtr > _renderQueue;
  Usul::Atomic::Container < JobList > _renderJobs;
  Usul::Atomic::Object < IContext::QueryPtr > _context;
  Usul::Atomic::Object < Node::RefPtr > _scene;
  Usul::Atomic::Object < IMatrixGet::QueryPtr > _projection;
  Usul::Atomic::Object < IMatrixGet::QueryPtr > _navigation;
  Usul::Atomic::Object < ClockTics > _timeAllowed;
  Usul::Atomic::Object < Viewport > _viewport;
};


} // namespace Viewers
} // namespace SceneGraph


#endif // _SCENE_GRAPH_VIEWER_CLASS_H_
