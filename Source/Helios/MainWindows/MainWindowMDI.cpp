
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Derived from earlier work by Adam Kubach and Perry Miller found here:
//  http://sourceforge.net/projects/cadkit/
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/MainWindows/MainWindowMDI.h"
#include "Helios/ChildWindows/ViewContainer.h"

#include "Helios/Menus/Separator.h"

#include "Tools/Qt/FileDialog.h"

#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IViewCreate.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Plugins/Manager.h"

#include "QtGui/QAction"
#include "QtGui/QCloseEvent"
#include "QtGui/QMdiArea"
#include "QtGui/QMdiSubWindow"

#include "boost/bind.hpp"

#include <memory>

using namespace Helios::MainWindows;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindowMDI::MainWindowMDI() : BaseClass(),
  _mdiArea ( new QMdiArea )
{
  // The central widget is an mdi area.
  this->setCentralWidget ( _mdiArea );

  // Add menu entries to manage mdi windows.
  Menus::MenuBar::RefPtr menuBar ( this->getMenuBar() );
  Menus::Menu::RefPtr menu ( new Menus::Menu ( "&Window" ) );
  menu->append ( new Helios::Menus::Button ( "&Cascade", "", "Cascade the child windows",
    boost::bind ( &QMdiArea::cascadeSubWindows, _mdiArea ), 
    boost::bind ( &MainWindowMDI::_updateMdiMenu, this, _1 ) ) );
  menu->append ( new Helios::Menus::Button ( "&Tile", "", "Tile the child windows",
    boost::bind ( &QMdiArea::tileSubWindows, _mdiArea ), 
    boost::bind ( &MainWindowMDI::_updateMdiMenu, this, _1 ) ) );
  menu->append ( new Helios::Menus::Button ( "Close Active Window", "", "Close the window that is currently active",
    boost::bind ( &QMdiArea::closeActiveSubWindow, _mdiArea ), 
    boost::bind ( &MainWindowMDI::_updateMdiMenu, this, _1 ) ) );
  menu->append ( new Helios::Menus::Button ( "Close All Windows", "", "Close every child window",
    boost::bind ( &QMdiArea::closeAllSubWindows, _mdiArea ), 
    boost::bind ( &MainWindowMDI::_updateMdiMenu, this, _1 ) ) );
  menu->append ( new Menus::Separator() );
  menuBar->append ( menu );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindowMDI::~MainWindowMDI()
{
  Usul::Functions::noThrow ( boost::bind ( &MainWindowMDI::_destroy, this ), "3967913269" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_destroy()
{
  this->threadCheck().throwIfDifferentThread();
  delete _mdiArea; _mdiArea = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the interface for creating views.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class InterfaceType > 
  inline Usul::Interfaces::IUnknown::RefPtr findInterface 
    ( Usul::Interfaces::IUnknown::RefPtr document )
  {
    // First try the given pointer.
    typedef Usul::Interfaces::IViewCreate IViewCreate;
    IViewCreate::QueryPtr create ( document );
    if ( true == create.valid() )
    {
      if ( true == create->canCreateView ( document ) )
      {
        return create;
      }
    }

    // Now try the plugin manager.
    create = IViewCreate::QueryPtr ( Usul::Plugins::Manager::instance().findFirst ( IViewCreate::IID ).get() );
    if ( true == create.valid() )
    {
      if ( true == create->canCreateView ( document ) )
      {
        return create;
      }
    }

    // If we get to here then it didn't work.
    return Usul::Interfaces::IUnknown::RefPtr();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_createGui ( Usul::Interfaces::IUnknown::RefPtr document )
{
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IViewCreate IViewCreate;
  typedef Usul::Documents::Document Document;
  typedef Helios::ChildWindows::ViewContainer ViewContainer;

  // Only the thread we were created in.
  this->threadCheck().throwIfDifferentThread();

  // Get needed interface to create the view. Note: it is not necessarily 
  // an error if there is no delegate; some documents may not need one.
  IViewCreate::QueryPtr create ( Helper::findInterface<IViewCreate> ( document ).get() );
  if ( false == create.valid() )
    return;

  // Create the view's container.
  std::auto_ptr < ViewContainer > viewParent ( new ViewContainer ( document ) );
  create->createView ( viewParent.get(), document, IUnknown::QueryPtr ( this ) );

  // Make mdi sub-window.
  std::auto_ptr < QMdiSubWindow > mdiChild ( _mdiArea->addSubWindow ( viewParent.get() ) );

  // Initialize view-container.
  viewParent->init ( mdiChild.get() );

  // If we get to here then show it and release it.
  this->_showChildWindow ( mdiChild.get() );
  mdiChild.release();
  viewParent.release();

  // Add document to the manager.
  Usul::Documents::Manager::instance().addDocument ( document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if there are subwindows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_updateMdiMenu ( QAction &action ) const
{
  this->threadCheck().throwIfDifferentThread();
  action.setEnabled ( ( 0x0 == _mdiArea ) ? false : ( 0x0 != _mdiArea->activeSubWindow() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mdi area.
//
///////////////////////////////////////////////////////////////////////////////

QMdiArea *MainWindowMDI::_getMdiArea()
{
  this->threadCheck().throwIfDifferentThread();
  return _mdiArea;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mdi area.
//
///////////////////////////////////////////////////////////////////////////////

const QMdiArea *MainWindowMDI::_getMdiArea() const
{
  this->threadCheck().throwIfDifferentThread();
  return _mdiArea;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the given mdi window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_showChildWindow ( QMdiSubWindow *window )
{
  this->threadCheck().throwIfDifferentThread();
  if ( 0x0 != window )
  {
    window->show();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_documentOpen()
{
  USUL_TRY_BLOCK
  {
    typedef Usul::Interfaces::IDocumentFilters::Filters Filters;
    typedef Tools::Qt::FileDialog FileDialog;
    typedef FileDialog::FileNames FileNames;

    this->threadCheck().throwIfDifferentThread();

    // Get all the filters.
    Filters filters;
    Usul::Documents::Manager::instance().filters ( "open", filters );

    // Pop up the file dialog.
    FileNames files;
    Tools::Qt::FileDialog::getOpenFileNames ( this, "Open", filters, files );

    // Add a job for each file.
    for ( FileNames::const_iterator i = files.begin(); i != files.end(); ++i )
    {
      const std::string file ( *i );
      Usul::Jobs::Queue::ValidRefPtr queue ( Usul::Jobs::Manager::instance()["worker_queue"] );
      queue->add ( boost::bind ( &MainWindowMDI::_documentOpen, this, file ) );
    }
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1207833167" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_documentOpen ( const std::string &file )
{
  BaseClass::_documentOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window is asked to close.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::closeEvent ( QCloseEvent *event )
{
  try
  {
    this->_closeSubWindows();
    event->accept();
  }
  catch ( const Usul::Exceptions::Cancelled & )
  {
    event->ignore();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close all the sub-windows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowMDI::_closeSubWindows()
{
  QList<QMdiSubWindow *> subWindows ( this->_getMdiArea()->subWindowList() );
  while ( false == subWindows.empty() )
  {
    QMdiSubWindow *subWindow ( subWindows.front() );
    subWindows.pop_front();
    if ( 0x0 != subWindow )
    {
      // Try to close this window.
      if ( false == subWindow->close() )
      {
        throw Usul::Exceptions::Cancelled();
      }
    }
  }
}
