
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Qt-based viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_DOCUMENT_VIEWER_H_
#define _MINERVA_DOCUMENT_VIEWER_H_

#include "Minerva/Document/CompileGuard.h"
#include "Minerva/Document/Document.h"

#include "Helios/Menus/Button.h"

#include "SceneGraph/Interfaces/IContext.h"
#include "SceneGraph/State/StateSet.h"
#include "SceneGraph/Viewers/Viewer.h"

#include "Usul/Atomic/Bool.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IDocumentGet.h"
#include "Usul/Interfaces/IModifiedListener.h"
#include "Usul/Interfaces/IPropertyEditorAccept.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Math/Quaternion.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Threads/Check.h"
#include "Usul/Types/Types.h"

#include "QtOpenGL/QGLWidget"

#include "boost/noncopyable.hpp"

#include <list>
#include <vector>


namespace Minerva {


class Viewer : 
  public QGLWidget,
  public boost::noncopyable,
  public SceneGraph::Interfaces::IContext,
  public Usul::Interfaces::IActiveViewListener,
  public Usul::Interfaces::IDocumentGet,
  public Usul::Interfaces::IModifiedListener,
  public Usul::Interfaces::IPropertyEditorAccept
{
  Q_OBJECT

public:

  // Typedefs
  typedef QGLWidget BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IDocumentGet IDocumentGet;
  typedef Usul::Interfaces::IModifiedListener IModifiedListener;
  typedef Usul::Interfaces::IPropertyEditorAccept IPropertyEditorAccept;
  typedef Usul::Documents::Document BaseDocument;
  typedef Minerva::Document::MouseEventData MouseEventData;
  typedef Usul::Math::Matrix44d Matrix;
  typedef std::pair < Usul::Math::Vec4i, bool > Viewport;
  typedef Usul::Jobs::Handle JobHandle;
  typedef SceneGraph::Draw::Method DrawMethod;
  typedef SceneGraph::Viewers::Viewer InternalViewer;
  typedef SceneGraph::Base::TimedObject::ClockTics ClockTics;
  typedef Usul::Jobs::Queue JobQueue;
  typedef Usul::Jobs::BaseJob Job;
  typedef Usul::Jobs::Queue::JobSet JobList;
  typedef Helios::Menus::Button Button;
  typedef Helios::Menus::Item MenuItem;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef Usul::Math::Vec2i Vec2i;
  typedef Usul::Math::Vector2 < Vec2i > MouseCoordinates;
  typedef Usul::Math::Vec4f Vec4f;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Quatd Quatd;
  typedef std::vector < double > IndepVars;
  typedef std::vector < MenuItem::RefPtr > MenuItems;
  typedef SceneGraph::State::StateSet StateSet;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Viewer );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor
  Viewer ( QWidget *parent, Minerva::Document::RefPtr doc );
  virtual ~Viewer();

  // Usul::Interfaces::IActiveViewListener
  virtual void                activeViewChanged ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView );

  // Animate from one position and orientation to the other.
  void                        animate ( const Matrix &m1, const Matrix &m2, unsigned int numSteps, ClockTics timeAllowedPerFrame, bool useSine, const Vec2i &p1 = Vec2i ( 0, 0 ), const Vec2i &p2 = Vec2i ( 0, 0 ) );
  void                        animate ( const Vec3d &t1, const Quatd &r1, const Vec3d &t2, const Quatd &r2, unsigned int numSteps, ClockTics timeAllowedPerFrame, bool useSine, const Vec2i &p1 = Vec2i ( 0, 0 ), const Vec2i &p2 = Vec2i ( 0, 0 ) );
  void                        animate ( const Vec3d &t1, const Quatd &r1, const Vec3d &t2, const Quatd &r2, const IndepVars &, ClockTics timeAllowedPerFrame, const Vec2i &p1 = Vec2i ( 0, 0 ), const Vec2i &p2 = Vec2i ( 0, 0 ) );

  // Get/set the background color.
  void                        backgroundColorSet ( const Vec4f & );
  Vec4f                       backgroundColorGet() const;

  // Set/get flags for drawing.
  void                        drawBoundingSpheresSet ( bool state );
  bool                        drawBoundingSpheresGet() const;

  // Cancel the rendering. Does nothing if it is not currently rendering.
  void                        cancelRender();

  // SceneGraph::Interfaces::IContext
  virtual unsigned long       contextId() const;
  virtual void                makeContextCurrent();
  virtual void                swapRenderingBuffers();

  // Usul::Interfaces::IDocumentGet
  const BaseDocument *        documentGet() const;
  BaseDocument *              documentGet();

  // Get/set the navigation matrix.
  Matrix                      navigationMatrixGet() const;
  void                        navigationMatrixSet ( const Matrix & );

  // Usul::Interfaces::IPropertyEditorAccept
  void                        propertyEditorAccept ( IUnknown::RefPtr );

  // Functions for working with render jobs.
  void                        renderJobAdd ( const Matrix &nav, ClockTics timeAllowed );
  void                        renderJobsCancel();
  unsigned int                renderJobsCount() const;

  // Get the state-set.
  StateSet::RefPtr            stateSet ( bool createIfNeeded = true );

  // Usul::Interfaces::IModifiedListener
  virtual void                subjectModified ( IUnknown::RefPtr caller = IUnknown::RefPtr() );

  // Navigate to where the entire scene is visible.
  void                        viewAll();

protected:

  // Override these events.
  virtual void                dragEnterEvent ( QDragEnterEvent * );
  virtual void                dropEvent ( QDropEvent * );
  virtual void                focusInEvent ( QFocusEvent * );
  virtual void                focusOutEvent ( QFocusEvent * );
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

  void                        _inertialJobAdd ( const MouseEventData & );
  void                        _inertialMotion ( Job::RefPtr job, const MouseEventData & );

  void                        _menuButtonsAdd();
  void                        _menuButtonsRemove();
  void                        _mouseInterpolate ( const Vec2i &m1, const Vec2i &m2, double u );
  void                        _mouseMoveEvent ( const MouseEventData & );

  void                        _render ( const Matrix &nav, ClockTics timeAllowed, Job::RefPtr job );
  void                        _renderJobFinished ( Job::RefPtr job );

  void                        _toolBarButtonsAdd();
  void                        _toolBarButtonsRemove();

private:

  void                        _destroy();

  Usul::Threads::Check _threadCheck;
  Usul::Atomic::Integer < unsigned long > _refCount;
  Usul::Atomic::Object < Minerva::Document::RefPtr > _document;
  Usul::Atomic::Object < Matrix > _projection;
  Usul::Atomic::Object < Matrix > _navigation;
  Usul::Atomic::Object < MouseCoordinates > _mouse;
  Usul::Atomic::Integer < int > _buttons;
  Usul::Atomic::Object < Viewport > _viewport;
  Usul::Atomic::Container < JobList > _renderJobs;
  Usul::Atomic::Container < JobList > _updateJobs;
  Usul::Atomic::Object < InternalViewer::RefPtr > _internalViewer;
  Usul::Atomic::Object < JobQueue::RefPtr > _renderQueue;
  Usul::Atomic::Container < MenuItems > _viewMenuItems;
  Usul::Atomic::Object < Job::RefPtr > _inertialJob;
  Usul::Atomic::Object < double > _mouseWheelSensitivity;
  Usul::Atomic::Object < Vec4f > _backgroundColor;
  Usul::Atomic::Bool _drawGeometry;
  Usul::Atomic::Bool _drawBoundingSpheres;
  Usul::Atomic::Object < StateSet::RefPtr > _stateSet;
};


} // namespace Minerva


#endif // _MINERVA_DOCUMENT_VIEWER_H_
