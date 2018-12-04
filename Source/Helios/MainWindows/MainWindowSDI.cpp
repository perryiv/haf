
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
//  SDI main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/MainWindows/MainWindowSDI.h"
#include "Helios/Menus/Separator.h"

#include "Tools/Qt/FileDialog.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IDocumentFilters.h"
#include "Usul/Jobs/Manager.h"

#include "QtGui/QMdiArea"
#include "QtGui/QMdiSubWindow"

#include "boost/bind.hpp"

using namespace Helios::MainWindows;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindowSDI::MainWindowSDI() : BaseClass()
{
  // Rebuild the window menu.
  Menus::MenuBar::RefPtr menuBar ( this->getMenuBar() );
  Menus::Menu::RefPtr menu ( menuBar->menu ( "&Window" ) );
  menu->clear();
  menu->append ( new Helios::Menus::Button ( "Close Window", "", "Close the document's window",
    boost::bind ( &QMdiArea::closeAllSubWindows, this->_getMdiArea() ), 
    boost::bind ( &MainWindowSDI::_updateMdiMenu, this, _1 ) ) );
  menu->append ( new Menus::Separator() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindowSDI::~MainWindowSDI()
{
  Usul::Functions::noThrow ( boost::bind ( &MainWindowSDI::_destroy, this ), "2445711089" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_destroy()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_createGui ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  // Only the thread we were created in.
  this->threadCheck().throwIfDifferentThread();

  // Call base class's function.
  BaseClass::_createGui ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close existing document and create a new one.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_documentNew()
{
  USUL_TRY_BLOCK
  {
    // Try to close all the windows (and documents).
    this->_getMdiArea()->closeAllSubWindows();
    if ( false == this->_getMdiArea()->subWindowList().empty() )
      return;

    // Now call base class's function.
    BaseClass::_documentNew();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "6250838000" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_documentOpen()
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
    const std::string file ( Tools::Qt::FileDialog::getOpenFileName 
      ( this, "Open", filters ) );
    if ( true == file.empty() )
      return;

    // Do not open files that are already open.
    if ( true == this->_forwardExistingFile ( file ) )
      return;

    // Try to close all the windows (and documents).
    try { this->_closeSubWindows(); }
    catch ( const Usul::Exceptions::Cancelled & ) { return; }

    // Add a job for the file.
    Usul::Jobs::Queue::ValidRefPtr queue 
      ( Usul::Jobs::Manager::instance()["worker_queue"] );
    queue->add ( boost::bind ( &MainWindowSDI::_documentOpen, this, file ) );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2020255221" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_documentOpen ( const std::string &file )
{
  BaseClass::_documentOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the given mdi window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindowSDI::_showChildWindow ( QMdiSubWindow *window )
{
  this->threadCheck().throwIfDifferentThread();
  if ( 0x0 != window )
  {
    window->showMaximized();
  }
}
