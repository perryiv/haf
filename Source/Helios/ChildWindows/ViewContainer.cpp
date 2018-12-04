
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Parent for views.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/ChildWindows/ViewContainer.h"
#include "Helios/Application.h"
#include "Helios/MainWindows/MainWindowSDI.h"
#include "Helios/Menus/MenuBar.h"

#include "Tools/Qt/FileDialog.h"

#include "Usul/Documents/FileDocument.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IDocumentGet.h"
#include "Usul/Interfaces/IFileDocument.h"

#include "QtGui/QAction"
#include "QtGui/QCloseEvent"
#include "QtGui/QMdiArea"
#include "QtGui/QMdiSubWindow"
#include "QtGui/QMessageBox"

#include "boost/bind.hpp"

#include <stdexcept>

using namespace Helios::ChildWindows;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ViewContainer::ViewContainer ( IUnknown::RefPtr unknown ) : BaseClass(),
  _refCount ( 0 ),
  _document ( unknown ),
  _mdiSubWindow ( 0x0 ),
  _windowMenuButton ( Menus::Button::RefPtr ( 0x0 ) )
{
  // Get the real document.
  Usul::Interfaces::IDocumentGet::QueryPtr idoc ( unknown );
  Usul::Documents::Document::RefPtr doc ( ( true == idoc.valid() ) ? idoc->documentGet() : 0x0 );
  if ( true == doc.valid() )
  {
    // Add this instance as a view.
    doc->viewAdd ( IUnknown::QueryPtr ( this ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::init ( QMdiSubWindow *mdiSubWindow )
{
  this->threadCheck().throwIfDifferentThread();
  _mdiSubWindow = mdiSubWindow;

  // Get the document's title.
  Usul::Interfaces::IDocumentGet::QueryPtr idoc ( this->document().get() );
  Usul::Documents::Document::RefPtr doc ( ( true == idoc.valid() ) ? idoc->documentGet() : 0x0 );
  const std::string title ( ( true == doc.valid() ) ? doc->title() : std::string ( "Untitled" ) );

  // Set window title.
  _mdiSubWindow->setWindowTitle ( title.c_str() );

  // Get the main window.
  typedef MainWindows::MainWindow MainWindow;
  typedef MainWindows::MainWindowSDI MainWindowSDI;
  MainWindow *mw ( Application::instance().mainWindow() );
  if ( 0x0 != mw )
  {
    // If it's not an SDI window...
    MainWindowSDI *sdi ( dynamic_cast < MainWindowSDI * > ( mw ) );
    if ( 0x0 == sdi )
    {
      // If the window menu already exists...
      Menus::MenuBar::RefPtr menuBar ( mw->getMenuBar() );
      Menus::Menu::RefPtr menu ( menuBar->at ( menuBar->find ( "&Window" ) ) );
      if ( true == menu.valid() )
      {
        _windowMenuButton = Helios::Menus::Button::RefPtr ( 
          new Helios::Menus::Button ( title, "", "Make this window the activate view",
          boost::bind ( &ViewContainer::makeActiveView, this ), 
          boost::bind ( &ViewContainer::_updateWindowMenuButton, this, _1 ) ) );
        menu->append ( _windowMenuButton );
        menuBar->dirty ( true );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ViewContainer::~ViewContainer()
{
  Usul::Functions::noThrow ( boost::bind ( &ViewContainer::_destroy, this ), "4016771153" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::_destroy()
{
  this->threadCheck().throwIfDifferentThread();

  // Remove menu entry.
  MainWindows::MainWindow *mw ( Application::instance().mainWindow() );
  if ( 0x0 != mw )
  {
    Menus::MenuBar::RefPtr menuBar ( mw->getMenuBar() );
    Menus::Menu::RefPtr menu ( menuBar->at ( menuBar->find ( "&Window" ) ) );
    if ( true == menu.valid() )
      menu->remove ( _windowMenuButton );
    _windowMenuButton = Menus::Button::RefPtr ( 0x0 );
    menuBar->dirty ( true );
  }

  // Need local scope or else the document is not purged.
  {
    // Copy and set the document.
    IUnknown::RefPtr unknown ( _document.fetchAndStore ( IUnknown::RefPtr ( 0x0 ) ) );

    // Try to get the real document.
    Usul::Interfaces::IDocumentGet::QueryPtr idoc ( unknown );
    Usul::Documents::Document::RefPtr doc ( ( true == idoc.valid() ) ? idoc->documentGet() : 0x0 );
    if ( true == doc.valid() )
    {
      // Remove this view from the document. Note: the ref/unref in this 
      // class does nothing, so this is safe.
      doc->viewRemove ( IUnknown::QueryPtr ( this ) );
    }
  }

  // Purge the documents to possibly free up space.
  Usul::Documents::Manager::instance().purge();

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ViewContainer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IDocumentView::IID:
  case Usul::Interfaces::IUnknown::IID:
    return static_cast < Usul::Interfaces::IDocumentView * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr ViewContainer::document()
{
  return _document;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets called by Qt to suggest a size.
//
///////////////////////////////////////////////////////////////////////////////

QSize ViewContainer::sizeHint() const
{
  this->threadCheck().throwIfDifferentThread();

  // Get main-window's central widget.
  MainWindows::MainWindow *mw ( Application::instance().mainWindow() );
  QWidget *widget ( ( 0x0 != mw ) ? mw->centralWidget() : 0x0 );
  if ( 0x0 == widget )
    return QSize ( -1, -1 );

  // Make it a standard size if possible, otherwise make it fit.
  QSize size ( 1280, 720 );
  const QRect pr ( widget->rect() );
  if ( ( size.width() >= pr.width() ) || ( size.height() >= pr.height() ) )
  {
    const double fraction ( 0.8 );
    size = QSize ( fraction * pr.width(), fraction * pr.height() );
  }
  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if we can close.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::_close()
{
  // If there is no document then there is nothing to do.
  typedef Usul::Interfaces::IDocumentGet IDocumentGet;
  typedef Usul::Documents::Document Document;
  IDocumentGet::QueryPtr idoc ( this->document().get() );
  Document::RefPtr doc ( ( true == idoc.valid() ) ? idoc->documentGet() : 0x0 );
  if ( false == doc.valid() )
    return;

  // If there is more than one view then there is nothing to do.
  if ( doc->viewCount() > 1 )
    return;

  // If the document is not modified then there is nothing to do.
  if ( false == doc->isModified() )
    return;

  // Ask the user if they want to save the document.
  QMessageBox question;
  question.setWindowTitle ( "Save Document?" );
  question.setText ( Usul::Strings::format 
    ( "Save changes to document '", doc->title(), "'?" ).c_str() );
  question.setIcon ( QMessageBox::Question );
  question.setStandardButtons 
    ( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
  question.setDefaultButton ( QMessageBox::Yes );
  const int answer ( question.exec() );

  // Did they cancel?
  if ( QMessageBox::Cancel == answer )
    throw Usul::Exceptions::Cancelled();

  // If they chose no then there is nothing to do.
  if ( QMessageBox::No == answer )
    return;

  // If we get to here then the user wants to save.

  // Is it a file document?
  typedef Usul::Interfaces::IFileDocument IFileDocument;
  typedef Usul::Documents::FileDocument FileDocument;
  IFileDocument::QueryPtr ifdoc ( this->document().get() );
  FileDocument::RefPtr fdoc ( ( true == ifdoc.valid() ) ? ifdoc->fileDocument() : 0x0 );
  if ( true == fdoc.valid() )
  {
    // Is the file name valid?
    if ( false == fdoc->isFileNameValid() )
    {
      // Get all the filters.
      Usul::Interfaces::IDocumentFilters::Filters filters;
      Usul::Documents::Manager::instance().filters ( "save", filters );

      // Pop up the file dialog.
      const std::string file ( Tools::Qt::FileDialog::getSaveFileName 
        ( this, "Save", filters ) );

      // Did the user cancel?
      if ( true == file.empty() )
        throw Usul::Exceptions::Cancelled();

      // Set the file name.
      FileDocument::FileInfo info ( file, true );
      fdoc->fileName ( info );
    }
  }

  // Tell the document to save itself.
  doc->save();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure we can close.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::closeEvent ( QCloseEvent *event )
{
  try
  {
    // This most likely will not happen, but handle events on the wrong thread.
    if ( true == this->threadCheck().isSameThread() )
    {
      // Try to close.
      this->_close();

      // We can close.
      event->accept();
    }
  }
  catch ( const Usul::Exceptions::Cancelled & )
  {
    event->ignore();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dereference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::unref ( bool )
{
  unsigned long previous ( _refCount.fetch_and_decrement() );
  if ( 0 == previous )
  {
    throw std::underflow_error ( "Error 3226443539: Decremented a reference count that was already zero" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make us the active view.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::makeActiveView()
{
  if ( 0x0 != _mdiSubWindow )
  {
    if ( 0x0 != _mdiSubWindow->mdiArea() )
    {
      _mdiSubWindow->mdiArea()->setActiveSubWindow ( _mdiSubWindow );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we the active view?
//
///////////////////////////////////////////////////////////////////////////////

bool ViewContainer::isActiveView() const
{
  if ( 0x0 != _mdiSubWindow )
  {
    if ( 0x0 != _mdiSubWindow->mdiArea() )
    {
      return ( _mdiSubWindow->mdiArea()->activeSubWindow() == _mdiSubWindow );
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu button.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::_updateWindowMenuButton ( QAction &action ) const
{
  action.setCheckable ( true );
  action.setChecked ( true == this->isActiveView() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the focus.
//
///////////////////////////////////////////////////////////////////////////////

void ViewContainer::bringToFront()
{
  typedef Usul::Jobs::Queue Queue;
  typedef Usul::Jobs::Manager Manager;

  if ( this->threadCheck().isSameThread() )
  {
    this->setFocus();
  }
  else
  {
    Queue::RefPtr queue ( Manager::instance()["idle_queue"] );
    if ( true == queue.valid() )
    {
      queue->add ( boost::bind ( &ViewContainer::bringToFront, this ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the thread-checking object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Check ViewContainer::threadCheck() const
{
  return Application::instance().threadCheck();
}
