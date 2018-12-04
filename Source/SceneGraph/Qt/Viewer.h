
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A thin wrapper around the internal viewer that simply translates events.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_QT_VIEWER_H_
#define _SCENE_GRAPH_QT_VIEWER_H_

#include "SceneGraph/Qt/Export.h"
#include "SceneGraph/Events/Events.h"
#include "SceneGraph/Interfaces/IContext.h"
#include "SceneGraph/Viewers/Viewer.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Interfaces/IModifiedListener.h"
#include "Usul/Threads/Check.h"

#include "QtOpenGL/QGLWidget"

#include "boost/noncopyable.hpp"


namespace SceneGraph {
namespace Qt {


class SCENE_GRAPH_QT_EXPORT Viewer : 
  public QGLWidget,
  public boost::noncopyable,
  public SceneGraph::Interfaces::IContext,
  public Usul::Interfaces::IModifiedListener
{
  Q_OBJECT

public:

  // Typedefs
  typedef QGLWidget BaseClass;
  typedef SceneGraph::Viewers::Viewer InternalViewer;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef SceneGraph::Events::Event::KeysDown KeysDown;
  typedef SceneGraph::Events::Event::ButtonsDown ButtonsDown;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Viewer );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor
  Viewer ( QWidget *parent, IUnknown::RefPtr document, IUnknown::RefPtr caller );
  virtual ~Viewer();

  // Return the set of keys that are currenty down.
  KeysDown                    keysDown() const;

  // SceneGraph::Interfaces::IContext
  virtual unsigned long       contextId() const;
  virtual void                makeContextCurrent();
  virtual void                swapRenderingBuffers();

  // Usul::Interfaces::IModifiedListener
  virtual void                subjectModified ( IUnknown::RefPtr caller = IUnknown::RefPtr() );

  // Tell the navigator to look at everything.
  virtual void                lookAtAll();

  // Return object for checking threads.
  Usul::Threads::Check        threadCheck() const;

protected:

  // Override these events.
  virtual void                keyPressEvent ( QKeyEvent * );
  virtual void                keyReleaseEvent ( QKeyEvent * );
  virtual void                mouseMoveEvent ( QMouseEvent * );
  virtual void                mousePressEvent ( QMouseEvent * );
  virtual void                mouseReleaseEvent ( QMouseEvent * );
  virtual void                mouseDoubleClickEvent ( QMouseEvent * );
  virtual void                paintEvent ( QPaintEvent * );
  virtual void                paintGL();
  virtual void                resizeEvent ( QResizeEvent * );
  virtual void                resizeGL ( int width, int height );
  virtual void                wheelEvent ( QWheelEvent * );

private:

  void                        _destroy();

  void                        _notify ( SceneGraph::Events::Event::RefPtr );

  Usul::Threads::Check _threadCheck;
  Usul::Atomic::Integer < unsigned long > _refCount;
  Usul::Atomic::Object < InternalViewer::RefPtr > _internalViewer;
  Usul::Atomic::Container < KeysDown > _keysDown;
  Usul::Atomic::Object < IUnknown::RefPtr > _document;
};


} // namespace Qt
} // namespace SceneGraph


#endif // _SCENE_GRAPH_QT_VIEWER_H_
