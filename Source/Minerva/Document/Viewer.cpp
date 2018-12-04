
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

#include "Minerva/Document/Viewer.h"
#include "Minerva/Document/Constants.h"
#include "Minerva/Document/Jobs.h"

#include "Helios/MainWindows/MainWindow.h"
#include "Helios/Menus/Functors.h"
#include "Helios/Menus/MenuBar.h"
#include "Helios/Menus/Separator.h"
#include "Helios/Menus/ToolBar.h"
#include "Helios/Application.h"

#include "SceneGraph/Common/Scope.h"
#include "SceneGraph/Interfaces/IRendererCreate.h"
#include "SceneGraph/State/Attributes/Attributes.h"

#include "Usul/Adapters/Compare.h"
#include "Usul/Adapters/Compose.h"
#include "Usul/Adapters/If.h"
#include "Usul/Adapters/Ignore.h"
#include "Usul/Adapters/Not.h"
#include "Usul/Adapters/Value.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Convert/Quaternion.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IPropertyEditor.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/NaN.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/Reset.h"
#include "Usul/System/Sleep.h"

#include "QtCore/QUrl"
#include "QtGui/QDragEnterEvent"
#include "QtGui/QHBoxLayout"

#include "boost/bind.hpp"

#include "GL/gl.h"

#include <stdexcept>

using namespace Minerva;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( QWidget *parent, Minerva::Document::RefPtr doc ) : 
  BaseClass ( QGLFormat::defaultFormat(), parent ),
  _threadCheck(),
  _refCount ( 0 ),
  _document ( doc ),
  _projection ( Matrix::getIdentity() ),
  _navigation ( Matrix::getIdentity() ),
  _mouse ( MouseCoordinates ( Vec2i ( 0, 0 ), Vec2i ( 0, 0 ) ) ),
  _buttons ( 0 ),
  _viewport ( Viewport ( Usul::Math::Vec4i ( 0, 0, 100, 100 ), true ) ),
  _renderJobs(),
  _updateJobs(),
  _renderer(),
  _internalViewer(),
  _renderQueue ( Usul::Jobs::Manager::instance()["render_queue"] ),
  _viewMenuItems(),
  _inertialJob(),
  _mouseWheelSensitivity ( 5 ),
  _backgroundColor ( Vec4f ( 0, 0, 0, 1 ) ),
  _drawGeometry ( true ),
  _drawBoundingSpheres ( false ),
  _stateSet()
{
  // Reference this class and dereference without deleting.
  NoDeleteRefPtr me ( this );

  // Needed below.
  Helios::MainWindows::MainWindow::RefPtr mw 
    ( Helios::Application::instance().mainWindow() );
  if ( false == mw.valid() )
    throw std::runtime_error ( "Error 2695650240: No main window" );

  // We'll handle swapping buffers.
  this->setAutoBufferSwap ( false );

  // Set the focus policy.
  this->setFocusPolicy ( Qt::ClickFocus );

  // Delete on close.
  this->setAttribute ( Qt::WA_DeleteOnClose );

  // Enable drag-and-drop.
  this->setAcceptDrops ( true );

  // Make the renderer and pass our address as the id.
  typedef SceneGraph::Interfaces::IRendererCreate IRendererCreate;
  IRendererCreate::QueryPtr rc 
    ( Usul::Plugins::Manager::instance().findFirst ( IRendererCreate::IID ) );
  if ( false == rc.valid() )
    throw std::runtime_error ( "Error 3104794598: No plugin found that will create the renderer" );
  _renderer = rc->createRenderer ( IUnknown::QueryPtr ( this ) );
  if ( false == _renderer.getReference().valid() )
    throw std::runtime_error ( "Error 1907424649: No renderer created" );

  // Make the rendering pipeline.
  typedef SceneGraph::Viewers::Viewer::Pipeline Pipeline;
  Pipeline pipeline;
  pipeline.push_back ( _renderer );

  // Make the internal viewer and add the pipeline.
  _internalViewer = new SceneGraph::Viewers::Viewer ( IUnknown::QueryPtr ( this ) );
  _internalViewer.getReference()->pipelineSet ( pipeline );

  if ( true == doc.valid() )
  {
    // Add us as a listener.
    doc->modifiedListenerAdd ( IUnknown::QueryPtr ( this ) );

    // Set initial navigation matrix.
    this->navigationMatrixSet ( doc->navigationMatrix() );
  }

  // Make the view fill the view-container.
  std::auto_ptr<QHBoxLayout> layout ( new QHBoxLayout );
  layout->addWidget ( this );
  layout->setContentsMargins ( 0, 0, 0, 0 );
  layout->setSpacing ( 0 );
  parent->setLayout ( layout.release() );

  // Listen for when the active-view changes.
  Usul::Documents::Manager::instance().activeViewListenerAdd ( IUnknown::QueryPtr ( this ) );

  // This shortens the lines below.
  using namespace SceneGraph::State::Attributes;

  // Make menu buttons.
  _viewMenuItems.getReference().push_back
  ( 
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Zoom &All", 
        "zoom_all.png", 
        "Zoom all the way out",
        Usul::Adapters::compose
        ( 
          boost::bind ( &Viewer::renderJobsCancel, this ),
          boost::bind ( &Viewer::viewAll, this ) 
        )
      )
    )
  );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr ( new Helios::Menus::Separator )
  );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Draw Bounding Spheres",
        "draw_bounding_spheres.png",
        "Draw bounding spheres of the scene nodes",
        Usul::Adapters::compose
        ( 
          boost::bind
          (
            &Viewer::drawBoundingSpheresSet, this, 
            Usul::Adapters::not
            ( 
              boost::bind ( &Viewer::drawBoundingSpheresGet, this )
            )
          ),
          boost::bind ( &Viewer::update, this )
        ), 
        Usul::Adapters::compose
        (
          Helios::Menus::Functors::updateCheckState
          ( 
            boost::bind ( &Viewer::drawBoundingSpheresGet, this )
          ),
          boost::bind
          (
            &Helios::MainWindows::MainWindow::dirtyToolBars, mw.get()
          )
        )
      )
    )
  );

  PolygonMode::RefPtr polygonsFilled ( new PolygonMode ( Face::BOTH, Mode::FILLED ) );
  PolygonMode::RefPtr polygonsLines  ( new PolygonMode ( Face::BOTH, Mode::LINES  ) );
  PolygonMode::RefPtr polygonsPoints ( new PolygonMode ( Face::BOTH, Mode::POINTS ) );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Polygons filled", 
        "polygons_as_filled.png",
        "Draw polygons as filled",
        Usul::Adapters::compose
        (
          Usul::Adapters::ifThenElse
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsFilled
            ),
            boost::bind
            (
              &StateSet::attributeRemove,
              this->stateSet(),
              boost::cref ( typeid ( PolygonMode ) )
            ),
            boost::bind
            (
              &StateSet::attributeAdd,
              this->stateSet(),
              polygonsFilled
            )
          ),
          boost::bind ( &Viewer::update, this )
        ),
        Usul::Adapters::compose
        (
          Helios::Menus::Functors::updateCheckState
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsFilled
            )
          ),
          boost::bind
          (
            &Helios::MainWindows::MainWindow::dirtyToolBars, mw.get()
          )
        )
      )
    )
  );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Polygons as lines",
        "polygons_as_lines.png",
        "Draw polygons with lines",
        Usul::Adapters::compose
        (
          Usul::Adapters::ifThenElse
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsLines
            ),
            boost::bind
            (
              &StateSet::attributeRemove,
              this->stateSet(),
              boost::cref ( typeid ( PolygonMode ) )
            ),
            boost::bind
            (
              &StateSet::attributeAdd,
              this->stateSet(),
              polygonsLines
            )
          ),
          boost::bind ( &Viewer::update, this )
        ),
        Usul::Adapters::compose
        (
          Helios::Menus::Functors::updateCheckState
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsLines
            )
          ),
          boost::bind
          (
            &Helios::MainWindows::MainWindow::dirtyToolBars, mw.get()
          )
        )
      )
    )
  );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Polygons as points",
        "polygons_as_points.png",
        "Draw polygons with points",
        Usul::Adapters::compose
        (
          Usul::Adapters::ifThenElse
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsPoints
            ),
            boost::bind
            (
              &StateSet::attributeRemove,
              this->stateSet(),
              boost::cref ( typeid ( PolygonMode ) )
            ),
            boost::bind
            (
              &StateSet::attributeAdd,
              this->stateSet(),
              polygonsPoints
            )
          ),
          boost::bind ( &Viewer::update, this )
        ),
        Usul::Adapters::compose
        (
          Helios::Menus::Functors::updateCheckState
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              polygonsPoints
            )
          ),
          boost::bind
          (
            &Helios::MainWindows::MainWindow::dirtyToolBars, mw.get()
          )
        )
      )
    )
  );

  CullFace::RefPtr backFaceCulling ( new CullFace ( Face::BACK ) );

  _viewMenuItems.getReference().push_back
  (
    Helios::Menus::Item::RefPtr
    (
      new Button
      ( 
        "Back face culling", 
        "back_face_culling.png",
        "Do not draw the back faces of polygons",
        Usul::Adapters::compose
        (
          Usul::Adapters::ifThenElse
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              backFaceCulling
            ),
            boost::bind
            (
              &StateSet::attributeRemove,
              this->stateSet(),
              boost::cref ( typeid ( CullFace ) )
            ),
            boost::bind
            (
              &StateSet::attributeAdd,
              this->stateSet(),
              backFaceCulling
            )
          ),
          boost::bind ( &Viewer::update, this )
        ),
        Usul::Adapters::compose
        (
          Helios::Menus::Functors::updateCheckState
          (
            boost::bind
            (
              &StateSet::attributeHasValue, 
              this->stateSet(), 
              backFaceCulling
            )
          ),
          boost::bind
          (
            &Helios::MainWindows::MainWindow::dirtyToolBars, mw.get()
          )
        )
      )
    )
  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  Usul::Functions::noThrow ( boost::bind ( &Viewer::_destroy, this ), "3386230756" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel and wait for job.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void cancelAndWait ( Usul::Jobs::Queue::RefPtr queue, 
                              Viewer::Job::RefPtr job )
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
  _threadCheck.throwIfDifferentThread();

  // Stop listening for active-view changes.
  {
    IUnknown::NoDeleteQueryPtr hold ( this );
    {
      Usul::Documents::Manager::instance().activeViewListenerRemove
        ( IUnknown::RefPtr ( hold ) );
    }
  }

  // Remove our buttons.
  this->_menuButtonsRemove();
  this->_toolBarButtonsRemove();
  _viewMenuItems.clear();

  // Stop intertial motion.
  Job::RefPtr inertialJob ( _inertialJob.fetchAndStore ( Job::RefPtr() ) );
  Helper::cancelAndWait ( _renderQueue, inertialJob );

  // Cancel and wait for the render jobs.
  JobList jobs ( _renderJobs.fetchAndStore ( JobList() ) );
  std::for_each ( jobs.begin(), jobs.end(), boost::bind ( Helper::cancelAndWait, _renderQueue, _1 ) );

  // Done with these.
  _renderer = Renderer::RefPtr();
  _internalViewer = InternalViewer::RefPtr();

  // Need local scope or else the document is not purged.
  {
    // Note: the ref/unref in this class does nothing, so this is safe.
    IUnknown::NoDeleteQueryPtr me ( this );

    // Copy and set the document.
    Minerva::Document::RefPtr doc ( _document.fetchAndStore ( Minerva::Document::RefPtr ( 0x0 ) ) );

    // Set the active view to null if the existing one is this instance.
    Usul::Documents::Manager::instance().activeViewSetIf ( IUnknown::RefPtr ( 0x0 ),
      std::bind2nd ( std::equal_to < IUnknown::RefPtr >(), me ) );

    // If the document is valid...
    if ( true == doc.valid() )
    {
      // Remove us from the listeners.
      doc->modifiedListenerRemove ( me );
    }
  }

  // Purge the documents to possibly free up space.
  Usul::Documents::Manager::instance().purge();

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dereference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool )
{
  unsigned long previous ( _refCount.fetch_and_decrement() );
  if ( 0 == previous )
  {
    throw std::underflow_error ( "Error 7070909610: Decremented a reference count that was already zero" );
  }
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
  case Usul::Interfaces::IDocumentGet::IID:
    return static_cast < Usul::Interfaces::IDocumentGet * > ( this );
  case Usul::Interfaces::IModifiedListener::IID:
    return static_cast < Usul::Interfaces::IModifiedListener * > ( this );
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener * > ( this );
  case Usul::Interfaces::IPropertyEditorAccept::IID:
    return static_cast < Usul::Interfaces::IPropertyEditorAccept * > ( this );
  case SceneGraph::Interfaces::IContext::IID:
    return static_cast < SceneGraph::Interfaces::IContext * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document *Viewer::documentGet()
{
  Guard guard ( _document.mutex() );
  return _document.getReference().get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the document.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Documents::Document *Viewer::documentGet() const
{
  Guard guard ( _document.mutex() );
  return _document.getReference().get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the render job.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderJobAdd ( const Matrix &nav, ClockTics timeAllowed )
{
  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );
  if ( false == queue.valid() )
    return;

  // Lock and get shortcut because we assign.
  typedef Usul::Atomic::Container < JobList > AtomicJobs;
  AtomicJobs::Guard guard ( _renderJobs.mutex() );
  JobList &jobs ( _renderJobs.getReference() );

  // Add a new job.
  jobs.insert ( queue->add ( boost::bind 
    ( &Viewer::_render, this, nav, timeAllowed, _1 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call when the job finished.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_renderJobFinished ( Job::RefPtr job )
{
  // Lock and get shortcut because we modify.
  typedef Usul::Atomic::Container < JobList > AtomicJobs;
  AtomicJobs::Guard guard ( _renderJobs.mutex() );
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
//  Cancel the existing render jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderJobsCancel()
{
  // Stop intertial motion.
  Job::RefPtr inertialJob ( _inertialJob.fetchAndStore ( Job::RefPtr() ) );
  if ( true == inertialJob.valid() )
  {
    inertialJob->cancel();
  }

  // Get and reset the collection of jobs.
  JobList jobs ( _renderJobs.fetchAndStore ( JobList() ) );

  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );
  if ( false == queue.valid() )
    return;

  // Remove and cancel all our jobs.
  for ( JobList::iterator i = jobs.begin(); i != jobs.end(); ++i )
  {
    USUL_TRY_BLOCK
    {
      Job::RefPtr job ( *i );
      if ( true == job.valid() )
      {
        queue->remove ( job );
        job->cancel();
      }
    }
    USUL_DEFINE_CATCH_BLOCKS ( "1227007146" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_render ( const Matrix &navigation, ClockTics timeAllowed, Usul::Jobs::BaseJob::RefPtr job )
{
  // Always remove this job from our collection.
  Usul::Scope::Caller::RefPtr removeJob ( Usul::Scope::makeCaller
    ( boost::bind ( &Viewer::_renderJobFinished, this, job ) ) );

  // Always swap buffers.
  Usul::Scope::Caller::RefPtr swapBuffers ( Usul::Scope::makeCaller 
    ( boost::bind ( &Viewer::swapBuffers, this ) ) );

  // This thread needs to make the context current.
  this->makeCurrent();

  // Enable these modes.
  ::glEnable ( GL_DEPTH_TEST );

  // Set the clear-color.
  const Vec4f bg ( this->backgroundColorGet() );
  ::glClearColor ( bg[0], bg[1], bg[2], bg[3] );

  // Clear the background.
  ::glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

  // Set the viewport if it's dirty.
  const Viewport viewport ( _viewport );
  if ( true == viewport.second )
  {
    const Viewport::first_type &vp ( viewport.first );
    ::glViewport ( vp[0], vp[1], vp[2], vp[3] );
    _viewport = Viewport ( vp, false );
  }

  // Always set the projection matrix.
  const Matrix projection ( _projection );
  ::glMatrixMode ( GL_PROJECTION );
  ::glLoadMatrixd ( projection.get() );

  // Initialize the model-view matrix.
  ::glMatrixMode ( GL_MODELVIEW );
  ::glLoadMatrixd ( navigation.get() );

  // Prepare the renderer.
  Renderer::RefPtr renderer ( _renderer );
  if ( false == renderer.valid() )
    return;
  renderer->reset();
  renderer->matrixSet ( navigation );
  renderer->timeAllowedSet ( timeAllowed );
  renderer->jobSet ( job );
  renderer->drawGeometrySet ( _drawGeometry );
  renderer->drawBoundingSpheresSet ( _drawBoundingSpheres );

  // Always reset the job.
  Usul::Jobs::BaseJob::RefPtr nullJob ( 0x0 );
  Usul::Scope::Caller::RefPtr resetJob ( Usul::Scope::makeCaller
    ( boost::bind ( &Renderer::jobSet, renderer.get(), nullJob ) ) );

  // Render the scene. Eat timed-out exceptions.
  try
  {
    // Have to save state here before we apply the local state-set.
    SceneGraph::Scope::ServerAttributes sa ( GL_ALL_ATTRIB_BITS );
    SceneGraph::Scope::ClientAttributes ca ( GL_CLIENT_ALL_ATTRIB_BITS );

    // Render the state-set, if there is one.
    StateSet::RefPtr ss ( this->stateSet ( false ) );
    if ( true == ss.valid() )
      ss->accept ( *renderer );

    // Render the document.
    Minerva::Document::RefPtr doc ( _document );
    if ( true == doc.valid() )
      doc->accept ( *renderer );
  }
  catch ( const Usul::Exceptions::TimedOut & )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to paint.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintEvent ( QPaintEvent * )
{
  _threadCheck.throwIfDifferentThread();

  const Matrix nav ( this->navigationMatrixGet() );

  if ( this->renderJobsCount() < 2 )
    this->renderJobAdd ( nav, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeEvent ( QResizeEvent * )
{
  _threadCheck.throwIfDifferentThread();

  // Handle bad input.
  const int iw ( this->width()  );
  const int ih ( this->height() );
  if ( ( iw <= 0 ) || ( ih <= 0 ) )
    return;

  // Set projection matrix.
  const double dw ( static_cast < double > ( iw ) );
  const double dh ( static_cast < double > ( ih ) );
  const double zFar ( 10 * Minerva::Constants::WGS_84_RADIUS_EQUATOR );
  _projection = Matrix::perspective ( 45, dw / dh, 1000, zFar );

  // Set viewport object.
  const GLsizei sw ( static_cast < GLsizei > ( iw ) );
  const GLsizei sh ( static_cast < GLsizei > ( ih ) );
  _viewport = Viewport ( Usul::Math::Vec4i ( 0, 0, sw, sh ), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to paint. When this function is called the context is 
//  already current for the thread. Overloaded to make sure it's not used.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintGL()
{
  USUL_ASSERT ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to resize. When this function is called the context is 
//  already current for the thread. Overloaded to make sure it's not used.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeGL ( int width, int height )
{
  USUL_ASSERT ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window now has focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusInEvent ( QFocusEvent *e )
{
  _threadCheck.throwIfDifferentThread();

  // Call the base class first.
  BaseClass::focusInEvent ( e );

  // We are the active view.
  Usul::Documents::Manager::instance().activeViewSet ( IUnknown::QueryPtr ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window has lost the focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusOutEvent ( QFocusEvent *e )
{
  _threadCheck.throwIfDifferentThread();
  BaseClass::focusInEvent ( e );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Loops and feeds the mouse "event" to the document and renders.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_inertialMotion ( Job::RefPtr job, const MouseEventData &data )
{
  _threadCheck.throwIfSameThread();

  // Handle invalid job.
  if ( false == job.valid() )
    return;

  // Loop until we've been cancelled.
  while ( false == job->isCancelled() )
  {
    // Process the mouse "event".
    this->_mouseMoveEvent ( data );

    // Render in this thread.
    this->_render ( this->navigationMatrixGet(), 1000 / Constants::TARGET_FRAME_RATE, job );

    // Sleep if we haven't been cancelled.
    if ( false == job->isCancelled() )
    {
      Usul::System::Sleep::milliseconds ( 10 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_mouseMoveEvent ( const MouseEventData &data )
{
  // Get the document.
  Minerva::Document::RefPtr doc ( _document );
  if ( false == doc.valid() )
    return;

  // Redirect to document.
  doc->handleMouseMove ( data );

  // Set new matrix.
  const Matrix m ( doc->navigationMatrix() );
  this->navigationMatrixSet ( m );

  // Set new background color.
  this->backgroundColorSet ( doc->backgroundColor ( m ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseMoveEvent ( QMouseEvent *e )
{
  typedef Usul::Math::Vec2i Vec2i;
  typedef Usul::Math::Vec2d Vec2d;

  USUL_TRY_BLOCK
  {
    _threadCheck.throwIfDifferentThread();

    if ( 0x0 == e )
      return;

    // Are there any buttons down?
    if ( 0 == e->buttons() )
      return;

    // See if the mouse buttons are down.
    const bool left   ( true == e->buttons().testFlag ( Qt::LeftButton  ) );
    const bool middle ( true == e->buttons().testFlag ( Qt::MidButton   ) );
    const bool right  ( true == e->buttons().testFlag ( Qt::RightButton ) );

    // Get previous data.
    const MouseCoordinates mouse ( _mouse );

    // Prepare new data. Last mouse gets the previous current mouse.
    const Usul::Math::Vec2i lm ( mouse[1] );
    const Usul::Math::Vec2i cm ( e->x(), e->y() );
    const Usul::Math::Vec2i ws ( this->width(), this->height() );
    const MouseEventData data ( lm, cm, ws, left, middle, right, _projection );

    // Handle the event.
    this->_mouseMoveEvent ( data );

    // Replace member data.
    _mouse = MouseCoordinates ( lm, cm );
    _buttons = static_cast < int > ( e->buttons() );

    const Matrix nav ( this->navigationMatrixGet() );
    ClockTics timeAllowed ( 1000 / Constants::TARGET_FRAME_RATE );

    // Asynchronous render if there are no existing jobs.
    if ( this->renderJobsCount() < 2 )
      this->renderJobAdd ( nav, timeAllowed );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3723080959" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mousePressEvent ( QMouseEvent *e )
{
  USUL_TRY_BLOCK
  {
    _threadCheck.throwIfDifferentThread();

    // Check input.
    if ( 0x0 == e )
      return;

    // Cancel any running rendering jobs.
    this->renderJobsCancel();

    // Initialize interaction.
    const Vec2i pt ( e->x(), e->y() );
    _mouse = MouseCoordinates ( pt, pt );
    _buttons = static_cast < int > ( e->buttons() );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1284057617" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  USUL_TRY_BLOCK
  {
    _threadCheck.throwIfDifferentThread();

    // Check input.
    if ( 0x0 == e )
      return;

    // Punt if there are still buttons down.
    if ( 0 != e->buttons() )
      return;

    // If the mouse didn't move enough then just draw everything once.
    const MouseCoordinates mouse ( _mouse );
    const Usul::Math::Vec2d lmd ( mouse[1][0], mouse[1][1] );
    const Usul::Math::Vec2d cmd ( mouse[0][0], mouse[0][1] );
    if ( lmd.distance ( cmd ) < 2.0 )
    {
      const Matrix nav ( this->navigationMatrixGet() );

      // Asynchronous render if there are not too many jobs.
      if ( this->renderJobsCount() < 2 )
        this->renderJobAdd ( nav, 0 );

      return;
    }

    // Prepare new data.
    const unsigned int buttons ( _buttons );
    const bool left   ( true == Usul::Bits::has ( buttons, Qt::LeftButton  ) );
    const bool middle ( true == Usul::Bits::has ( buttons, Qt::MidButton   ) );
    const bool right  ( true == Usul::Bits::has ( buttons, Qt::RightButton ) );
    const Usul::Math::Vec2i ws ( this->width(), this->height() );
    const MouseEventData data ( mouse[0], mouse[1], ws, left, middle, right, _projection );

    // Add inertial job.
    this->_inertialJobAdd ( data );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1901185075" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been double-clicked.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseDoubleClickEvent ( QMouseEvent *e )
{
  USUL_TRY_BLOCK
  {
    _threadCheck.throwIfDifferentThread();

    // Check input.
    if ( 0x0 == e )
      return;

    // Get the document.
    Minerva::Document::RefPtr doc ( _document );
    if ( false == doc.valid() )
      return;

    // Check button state.
    const unsigned int buttons ( e->button() );
    const bool left  ( true == Usul::Bits::has ( buttons, Qt::LeftButton  ) );
    const bool right ( true == Usul::Bits::has ( buttons, Qt::RightButton ) );
    if ( ( false == left ) && ( false == right ) )
      return;

    // Get the existing matrix.
    const Matrix m1 ( this->navigationMatrixGet() );

    // Pretend the user dragged the mouse to the middle of the screen.
    const Usul::Math::Vec2i lm ( e->x(), e->y() );
    const Usul::Math::Vec2i cm ( this->width() / 2, this->height() / 2 );
    const Usul::Math::Vec2i ws ( this->width(), this->height() );
    const MouseEventData data ( lm, cm, ws, true, false, true, _projection );

    // Redirect to document.
    doc->handleMouseMove ( data );

    // Change the translation.
    doc->zoomFraction ( ( left ) ? 0.5 : -2.0 );
    doc->zoomClamp();

    // Get new matrix.
    const Matrix m2 ( doc->navigationMatrix() );

    // Animate between the two matrices.
    const unsigned int numSteps ( 2 * Constants::TARGET_FRAME_RATE );
    const ClockTics timeAllowed ( 1000 / Constants::TARGET_FRAME_RATE );
    this->animate ( m1, m2, numSteps, timeAllowed, true, lm, cm );

    // Add one more render job that takes the time it needs.
    this->renderJobAdd ( m2, 0 );

    // Since the "mouse release" event is likely going to fire also, 
    // reset the mouse data so we don't activate inertial motion.
    _mouse = MouseCoordinates ( lm, lm );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3703689090" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse wheel has been moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::wheelEvent ( QWheelEvent *e )
{
  USUL_TRY_BLOCK
  {
    _threadCheck.throwIfDifferentThread();

    // Check input.
    if ( 0x0 == e )
      return;

    // Pretend the mouse moved in the y-direction.
    const double delta ( static_cast < double > ( e->delta() ) );
    const int motion ( static_cast < int > ( delta / _mouseWheelSensitivity ) );

    // Prepare the data.
    const Usul::Math::Vec2i lm ( e->x(), motion );
    const Usul::Math::Vec2i cm ( e->x(), 0 );
    const Usul::Math::Vec2i ws ( this->width(), this->height() );
    const MouseEventData data ( lm, cm, ws, false, false, true, _projection );

    // Handle the event.
    this->_mouseMoveEvent ( data );

    // Asynchronous render.
    const Matrix nav ( this->navigationMatrixGet() );
    this->renderJobsCancel();
    this->renderJobAdd ( nav, 1000 / Constants::TARGET_FRAME_RATE );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1046048141" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyPressEvent ( QKeyEvent *e )
{
  _threadCheck.throwIfDifferentThread();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyReleaseEvent ( QKeyEvent *e )
{
  _threadCheck.throwIfDifferentThread();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dragging has entering window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dragEnterEvent ( QDragEnterEvent *event )
{
  _threadCheck.throwIfDifferentThread();

  Minerva::Document::RefPtr doc ( _document );
  if ( ( false == doc.valid() ) || ( 0x0 == event ) )
    return;

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Urls urls ( event->mimeData()->urls() );
  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    const std::string file ( i->toLocalFile().toStdString() );
    if ( true == doc->canInsert ( file ) )
    {
      event->acceptProposedAction();
      return;
    }
  }

  event->accept();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Files have been dropped.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dropEvent ( QDropEvent *event )
{
  _threadCheck.throwIfDifferentThread();

  Minerva::Document::RefPtr doc ( _document );
  Usul::Interfaces::IRead::QueryPtr reader ( doc );
  if ( ( false == reader.valid() ) || ( 0x0 == event ) )
    return;

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  Urls urls ( event->mimeData()->urls() );
  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    const std::string file ( i->toLocalFile().toStdString() );
    reader->read ( file, me );
  }

  event->acceptProposedAction();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the document is modified.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::subjectModified ( IUnknown::RefPtr doc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Navigate to where the entire scene is visible.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewAll()
{
  // Need the document.
  Minerva::Document::RefPtr doc ( _document );
  if ( false == doc.valid() )
    return;

  // Get the current rotation and translation.
  const Matrix m1 ( this->navigationMatrixGet() );
  doc->navigationReset();
  const Matrix m2 ( doc->navigationMatrix() );

  // If they are equal then there is nothing to do.
  if ( true == m1.equal ( m2 ) )
    return;

  // Animate.
  const unsigned int numSteps ( 3 * Constants::TARGET_FRAME_RATE );
  const ClockTics timeAllowed ( 1000 / Constants::TARGET_FRAME_RATE );
  this->animate ( m1, m2, numSteps, timeAllowed, true );

  // Add one more render job that takes the time it needs.
  this->renderJobAdd ( m2, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to the other.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::animate ( const Matrix &m1, const Matrix &m2, 
                       unsigned int numSteps, ClockTics timeAllowedPerFrame, 
                       bool useSine, const Vec2i &p1, const Vec2i &p2 )
{
  typedef Usul::Convert::Type < Matrix, Quatd > Converter;

  // Get the current rotation and translation.
  const Vec3d t1 ( m1.getTranslation<Vec3d>() );
  const Quatd r1 ( Converter::convert ( m1 ).normalized() );

  // Get the final rotation and translation from the document.
  const Vec3d t2 ( m2.getTranslation<Vec3d>() );
  const Quatd r2 ( Converter::convert ( m2 ).normalized() );

  // Call other animation function.
  this->animate ( t1, r1, t2, r2, numSteps, timeAllowedPerFrame, useSine, p1, p2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to the other.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::animate ( const Vec3d &t1, const Quatd &q1, 
                       const Vec3d &t2, const Quatd &q2, 
                       unsigned int numSteps, ClockTics timeAllowedPerFrame, 
                       bool useSine, const Vec2i &p1, const Vec2i &p2 )
{
  // Initialize the parameters.
  IndepVars params;
  params.reserve ( numSteps );

  // Generate parameters for the path.
  for ( unsigned int i = 0; i < numSteps; ++i )
  {
    double u ( static_cast<double> ( i ) / static_cast<double> ( numSteps - 1 ) );
    if ( true == useSine )
    {
      u = 0.5 * ( 1.0 + Usul::Math::sin ( u * Usul::Math::PI - Usul::Math::PI_OVER_2 ) );
    }
    params.push_back ( u );
  }

  // Call the other animation function.
  this->animate ( t1, q1, t2, q2, params, timeAllowedPerFrame, p1, p2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to the other.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::animate ( const Vec3d &t1, const Quatd &q1, 
                       const Vec3d &t2, const Quatd &q2, 
                       const IndepVars &params, ClockTics timeAllowedPerFrame,
                       const Vec2i &p1, const Vec2i &p2 )
{
  typedef Usul::Convert::Type < Quatd, Matrix > Converter;

  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );
  if ( false == queue.valid() )
    return;

  // Needed below.
  Minerva::Document::RefPtr doc ( _document );

  // Lock and get shortcut because we assign.
  typedef Usul::Atomic::Container < JobList > AtomicJobs;
  AtomicJobs::Guard guard ( _renderJobs.mutex() );
  JobList &jobs ( _renderJobs.getReference() );

  // Copy and normalize just to be sure.
  const Quatd r1 ( q1.normalized() );
  const Quatd r2 ( q2.normalized() );

  // Check numerical integrity.
  if ( ( true == r1.check ( &Usul::Math::nan<Quatd::value_type> ) ) ||
       ( true == r2.check ( &Usul::Math::nan<Quatd::value_type> ) ) )
  {
    USUL_ASSERT ( 0 );
    return;
  }

  // Generate matrices along a path by interpolating.
  for ( IndepVars::const_iterator i = params.begin(); i != params.end(); ++i )
  {
    // Make the matrix.
    const double u ( *i );
    const Vec3d t ( t1 + u * ( t2 - t1 ) );
    const Quatd r ( Quatd::slerp ( r1, r2, u ).normalized() );
    const Matrix m ( Matrix::translation ( t ) * Converter::convert ( r ) );

#if 0
    typedef Usul::Convert::Type<double,std::string> DoubleToString;
    std::cout << Usul::Strings::format ( 
      "Translation: ", 
        DoubleToString::convert ( t[0] ), ' ', 
        DoubleToString::convert ( t[1] ), ' ', 
        DoubleToString::convert ( t[2] ), ", ",
      "Rotation: Angle: ", 
        DoubleToString::convert ( r.angle() ), ", ",
      "Axis: ", 
        DoubleToString::convert ( r.axis()[0] ), ' ', 
        DoubleToString::convert ( r.axis()[1] ), ' ', 
        DoubleToString::convert ( r.axis()[2] ),
      '\n' ) << std::flush;
#endif

    // Check numerical integrity.
    if ( ( true == t.check ( &Usul::Math::nan<Quatd::value_type> ) ) ||
         ( true == r.check ( &Usul::Math::nan<Quatd::value_type> ) ) ||
         ( true == m.check ( &Usul::Math::nan<Quatd::value_type> ) ) )
    {
      USUL_ASSERT ( 0 );
      return;
    }

    // We add jobs in sequence to keep things as consistant as possible
    // in case the user interupts the animation.

    // Add job to set the navigation matrix.
    jobs.insert ( queue->add ( Usul::Adapters::compose (
      boost::bind ( &Viewer::_renderJobFinished, this, _1 ),
      Usul::Adapters::ignore ( 
      boost::bind ( &Viewer::navigationMatrixSet, this, m ) ) ) ) );

    // If there is a document, set the background color.
    if ( true == doc.valid() )
    {
      jobs.insert ( queue->add ( Usul::Adapters::compose (
        boost::bind ( &Viewer::_renderJobFinished, this, _1 ),
        Usul::Adapters::ignore ( 
        boost::bind ( &Viewer::backgroundColorSet, this,
        boost::bind<Vec4f> ( boost::mem_fn ( 
        &Minerva::Document::backgroundColor ), doc.get(), m ) ) ) ) ) );
    }

    // If we are given different mouse coordinates...
    if ( false == p1.equal ( p2 ) )
    {
      jobs.insert ( queue->add ( Usul::Adapters::compose (
        boost::bind ( &Viewer::_renderJobFinished, this, _1 ),
        Usul::Adapters::ignore (
        boost::bind ( &Viewer::_mouseInterpolate, this, p1, p2, u ) ) ) ) );
    }

    // Add a job to render.
    jobs.insert ( queue->add ( boost::bind 
      ( &Viewer::_render, this, m, timeAllowedPerFrame, _1 ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the active view changes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::activeViewChanged ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView )
{
  // Always remove.
  this->_menuButtonsRemove();
  this->_toolBarButtonsRemove();

  // Always cancel intertial motion.
  Job::RefPtr inertialJob ( _inertialJob.fetchAndStore ( Job::RefPtr() ) );
  if ( true == inertialJob.valid() )
    inertialJob->cancel();

  // If we are the new view then add our buttons.
  if ( this->queryInterface ( IUnknown::IID ) == newView.get() )
  {
    this->_menuButtonsAdd();
    this->_toolBarButtonsAdd();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add our menu buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_menuButtonsAdd()
{
  MenuItems items ( _viewMenuItems );
  if ( true == items.empty() )
    return;

  Helios::MainWindows::MainWindow::RefPtr mw ( Helios::Application::instance().mainWindow() );
  if ( false == mw.valid() )
    return;

  Helios::Menus::MenuBar::RefPtr menuBar ( mw->getMenuBar() );
  if ( false == menuBar.valid() )
    return;

  Helios::Menus::Menu::RefPtr menu ( menuBar->menu ( "&View", menuBar->find ( "&File" ) + 1 ) );

  for ( MenuItems::iterator i = items.begin(); i != items.end(); ++i )
  {
    Button::RefPtr button ( static_cast < Button * > ( i->get() ) );
    if ( true == button.valid() )
    {
      menu->append ( button );
      menuBar->dirty ( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove our menu buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_menuButtonsRemove()
{
  MenuItems items ( _viewMenuItems );
  if ( true == items.empty() )
    return;

  Helios::MainWindows::MainWindow::RefPtr mw ( Helios::Application::instance().mainWindow() );
  if ( false == mw.valid() )
    return;

  Helios::Menus::MenuBar::RefPtr menuBar ( mw->getMenuBar() );
  if ( false == menuBar.valid() )
    return;

  Helios::Menus::Menu::RefPtr menu ( menuBar->at ( menuBar->find ( "&View" ) ) );
  if ( false == menu.valid() )
    return;

  for ( MenuItems::iterator i = items.begin(); i != items.end(); ++i )
  {
    Button::RefPtr button ( static_cast < Button * > ( i->get() ) );
    if ( true == button.valid() )
    {
      menu->remove ( button );
      menuBar->dirty ( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add our toolbar buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_toolBarButtonsAdd()
{
  MenuItems items ( _viewMenuItems );
  if ( true == items.empty() )
    return;

  Helios::MainWindows::MainWindow::RefPtr mw ( Helios::Application::instance().mainWindow() );
  if ( false == mw.valid() )
    return;

  Helios::Menus::ToolBar::RefPtr toolBar ( mw->getToolBar ( "View" ) );
  if ( false == toolBar.valid() )
    return;

  for ( MenuItems::iterator i = items.begin(); i != items.end(); ++i )
  {
    Button::RefPtr button ( static_cast < Button * > ( i->get() ) );
    if ( true == button.valid() )
    {
      toolBar->append ( button );
      toolBar->needToRebuild ( true );
      toolBar->needToUpdate ( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove our toolbar buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_toolBarButtonsRemove()
{
  MenuItems items ( _viewMenuItems );
  if ( true == items.empty() )
    return;

  Helios::MainWindows::MainWindow::RefPtr mw ( Helios::Application::instance().mainWindow() );
  if ( false == mw.valid() )
    return;

  Helios::Menus::ToolBar::RefPtr toolBar ( mw->getToolBar ( "View" ) );
  if ( false == toolBar.valid() )
    return;

  for ( MenuItems::iterator i = items.begin(); i != items.end(); ++i )
  {
    Button::RefPtr button ( static_cast < Button * > ( i->get() ) );
    if ( true == button.valid() )
    {
      toolBar->remove ( button );
      toolBar->needToRebuild ( true );
      toolBar->needToUpdate ( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navigationMatrixSet ( const Matrix &m )
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
  return _navigation;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColorSet ( const Vec4f &c )
{
  _backgroundColor = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f Viewer::backgroundColorGet() const
{
  return _backgroundColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the intertial job.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_inertialJobAdd ( const MouseEventData &data )
{
  // Get the queue.
  Usul::Jobs::Queue::RefPtr queue ( _renderQueue );
  if ( false == queue.valid() )
    return;

  // Lock and get shortcut because we assign.
  typedef Usul::Atomic::Container < JobList > AtomicJobs;
  AtomicJobs::Guard g1 ( _renderJobs.mutex() );
  JobList &jobs ( _renderJobs.getReference() );

  // Lock and get shortcut because we assign.
  Guard g2 ( _inertialJob.mutex() );
  Job::RefPtr &inertialJob ( _inertialJob.getReference() );
  if ( true == inertialJob.valid() )
    return;

  // Make the job for intertial motion.
  Job::RefPtr job ( Usul::Jobs::makeJob ( 0, boost::bind ( 
    &Viewer::_inertialMotion, this, _1, data ) ) );

  // Add the job in this order.
  inertialJob = job;
  jobs.insert ( job );
  queue->add ( job );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the mouse coordinate by interpolating. This is called during 
//  double-click animations because the normal mouse-press reset of the 
//  coordinates do not happen, resulting in a "jump" of the geometry if 
//  the user clicks while the animation is still going.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_mouseInterpolate ( const Vec2i &m1, const Vec2i &m2, double u )
{
  typedef Usul::Math::Vec2d Vec2d;
  const Vec2d p1 ( m1[0], m1[1] );
  const Vec2d p2 ( m2[0], m2[1] );
  const Vec2d p ( p1 + u * ( p2 - p1 ) );
  const Vec2i m ( static_cast < int > ( p[0] ), static_cast < int > ( p[1] ) );
  _mouse = MouseCoordinates ( m, m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set flag for drawing.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::drawBoundingSpheresSet ( bool state )
{
  _drawBoundingSpheres = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get flag for drawing.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::drawBoundingSpheresGet() const
{
  return _drawBoundingSpheres;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state-set.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::StateSet::RefPtr Viewer::stateSet ( bool createIfNeeded )
{
  Guard guard ( _stateSet.mutex() );
  StateSet::RefPtr &ss ( _stateSet.getReference() );
  if ( false == ss.valid() )
  {
    if ( true == createIfNeeded )
    {
      ss = StateSet::RefPtr ( new StateSet );
    }
  }
  return ss;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the property editor.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::propertyEditorAccept ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  typedef Usul::Interfaces::IPropertyEditor IPropertyEditor;
  IPropertyEditor::QueryPtr editor ( unknown );
  if ( false == editor.valid() )
    return;

  editor->propertyAdd ( "Mouse Wheel Sensitivity", 
    boost::any ( static_cast < double > ( _mouseWheelSensitivity ) ),
    Usul::Interfaces::IUnknown::QueryPtr ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the context id.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Viewer::contextId() const
{
  return reinterpret_cast < unsigned int > ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this context current.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::makeContextCurrent()
{
  this->makeCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swapthe buffers
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::swapRenderingBuffers()
{
  this->swapBuffers();
}
