
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

#include "SceneGraph/Qt/Viewer.h"
#include "SceneGraph/Interfaces/INodeGet.h"
#include "SceneGraph/Interfaces/IDrawMethodCreate.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Plugins/Manager.h"

#include "QtGui/QApplication"
#include "QtGui/QHBoxLayout"
#include "QtGui/QResizeEvent"

#include "boost/bind.hpp"

#include <stdexcept>

using namespace SceneGraph::Qt;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( QWidget *parent, IUnknown::RefPtr document, IUnknown::RefPtr caller ) : 
  BaseClass ( QGLFormat::defaultFormat(), parent ),
  _threadCheck(),
  _refCount ( 0 ),
  _internalViewer(),
  _keysDown(),
  _document ( document )
{
  // Reference this class and dereference without deleting.
  NoDeleteRefPtr me ( this );

  // Has to be the same thread as the application.
  if ( QApplication::instance()->thread() != this->thread() )
  {
    throw Usul::Exceptions::Error 
      ( "1621621613", "Must create viewer in application thread" );
  }

  // We'll handle swapping buffers.
  this->setAutoBufferSwap ( false );

  // Set the focus policy.
  this->setFocusPolicy ( ::Qt::ClickFocus );

  // Delete on close.
  this->setAttribute ( ::Qt::WA_DeleteOnClose );

  // Make the internal viewer and add the pipeline.
  typedef SceneGraph::Viewers::Viewer InternalViewer;
  _internalViewer = new InternalViewer ( IUnknown::QueryPtr ( this ) );

  // Make the view fill the view-container.
  std::auto_ptr<QHBoxLayout> layout ( new QHBoxLayout );
  layout->addWidget ( this );
  layout->setContentsMargins ( 0, 0, 0, 0 );
  layout->setSpacing ( 0 );
  parent->setLayout ( layout.release() );

  // Add us as a listener to the document.
  typedef Usul::Interfaces::IModifiedSubject IModifiedSubject;
  IModifiedSubject::QueryPtr ms ( document );
  if ( true == ms.valid() )
  {
    ms->modifiedListenerAdd ( IUnknown::QueryPtr ( this ) );
  }

  // See if the document has a scene.
  typedef SceneGraph::Interfaces::INodeGet INodeGet;
  INodeGet::QueryPtr ng ( document );
  if ( true == ng.valid() )
  {
    INodeGet::NodePtr scene ( ng->nodeGet() );
    if ( true == scene.valid() )
    {
      _internalViewer.getReference()->sceneSet ( scene );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  Usul::Functions::noThrow ( boost::bind ( &Viewer::_destroy, this ), "3141450834" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this object.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_destroy()
{
  this->threadCheck().throwIfDifferentThread();

  // Done with these.
  _internalViewer = InternalViewer::RefPtr();
  _keysDown.clear();

  // Need local scope or else the document is not purged.
  {
    IUnknown::NoDeleteQueryPtr me ( this );

    // Copy and set the document.
    IUnknown::RefPtr doc ( _document.fetchAndStore ( IUnknown::RefPtr() ) );

    // Set the active view to null if the existing one is this instance.
    Usul::Documents::Manager::instance().activeViewSetIf ( IUnknown::RefPtr(),
      std::bind2nd ( std::equal_to < IUnknown::RefPtr >(), me ) );

    // Remove us as a listener.
    typedef Usul::Interfaces::IModifiedSubject IModifiedSubject;
    IModifiedSubject::QueryPtr ms ( doc );
    if ( true == ms.valid() )
    {
      // Remove us from the listeners.
      ms->modifiedListenerRemove ( me );
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
    throw std::underflow_error 
      ( "Error 2087950037: Decremented a reference count that was already zero" );
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
  case SceneGraph::Interfaces::IContext::IID:
    return static_cast < SceneGraph::Interfaces::IContext * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the mouse state.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class EventType > 
  inline Viewer::ButtonsDown buttonsDown ( EventType *e )
  {
    Viewer::ButtonsDown mouse;

    if ( 0x0 != e )
    {
      if ( true == e->buttons().testFlag ( Qt::LeftButton ) )
        mouse.insert ( 0 );
      if ( true == e->buttons().testFlag ( Qt::MidButton ) )
        mouse.insert ( 1 );
      if ( true == e->buttons().testFlag ( Qt::RightButton ) )
        mouse.insert ( 2 );
    }

    return mouse;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to paint.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintEvent ( QPaintEvent * )
{
  this->threadCheck().throwIfDifferentThread();
  typedef SceneGraph::Events::Paint Event;
  this->_notify ( Event::RefPtr ( new Event ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by Qt to resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeEvent ( QResizeEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::Resize Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->size().width(), e->size().height() ) ) );
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
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseMoveEvent ( QMouseEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::MouseMove Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->x(), e->y(), this->keysDown(), Helper::buttonsDown ( e ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mousePressEvent ( QMouseEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::MousePress Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->x(), e->y(), e->button(), this->keysDown(), Helper::buttonsDown ( e ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::MouseRelease Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->x(), e->y(), e->button(), this->keysDown(), Helper::buttonsDown ( e ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been double-clicked.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseDoubleClickEvent ( QMouseEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::MouseDoubleClick Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->x(), e->y(), e->button(), this->keysDown(), Helper::buttonsDown ( e ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse wheel has been moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::wheelEvent ( QWheelEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  typedef SceneGraph::Events::MouseWheel Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->x(), e->y(), e->delta(), this->keysDown(), Helper::buttonsDown ( e ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyPressEvent ( QKeyEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  {
    Guard guard ( _keysDown.mutex() );
    _keysDown.getReference().insert ( e->key() );
  }

  typedef SceneGraph::Events::KeyPress Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->key(), this->keysDown(), Viewer::ButtonsDown() ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyReleaseEvent ( QKeyEvent *e )
{
  this->threadCheck().throwIfDifferentThread();

  if ( 0x0 == e )
    return;

  {
    Guard guard ( _keysDown.mutex() );
    _keysDown.getReference().erase ( e->key() );
  }

  typedef SceneGraph::Events::KeyRelease Event;
  this->_notify ( Event::RefPtr ( new Event 
    ( e->key(), this->keysDown(), Viewer::ButtonsDown() ) ) );
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
//  Swap the buffers
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::swapRenderingBuffers()
{
  this->swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the keys down.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::KeysDown Viewer::keysDown() const
{
  return _keysDown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the event to the internal viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_notify ( SceneGraph::Events::Event::RefPtr e )
{
  if ( false == e.valid() )
    return;

  InternalViewer::RefPtr iv ( _internalViewer );
  if ( false == iv.valid() )
    return;

  iv->notify ( e );
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
//  Tell the navigator to look at everything.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lookAtAll()
{
  InternalViewer::RefPtr iv ( _internalViewer );
  if ( true == iv.valid() )
  {
    iv->lookAtAll();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the thread-checking object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Check Viewer::threadCheck() const
{
  return _threadCheck;
}
