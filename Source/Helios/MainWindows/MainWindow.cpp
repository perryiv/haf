
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/MainWindows/MainWindow.h"
#include "Helios/Application.h"
#include "Helios/Menus/Action.h"
#include "Helios/Menus/Functors.h"
#include "Helios/Menus/Menu.h"
#include "Helios/Menus/Separator.h"

#include "Tools/Qt/FileDialog.h"
#include "Tools/Qt/Functions.h"

#include "Tree/Registry/IO.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Commands/Commands.h"
#include "Usul/Convert/Qt.h"
#include "Usul/Documents/FileDocument.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/AsciiInputFile.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/System/LastError.h"
#include "Usul/User/Directory.h"

#include "QtCore/QTimer"
#include "QtGui/QApplication"
#include "QtGui/QDockWidget"
#include "QtGui/QStatusBar"

#include "boost/bind.hpp"
#include "boost/filesystem.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/thread_time.hpp"

#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>

using namespace Helios::MainWindows;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass(),
  _refCount ( 0 ),
  _idleTimer ( 0x0 ),
  _menuBar(),
  _toolBars()
{
  using namespace Helios::Menus;

  // Has to be the same thread as the application.
  Application::instance().threadCheck().throwIfDifferentThread();

  // Only one main-window.
  Application::instance()._setMainWindow ( this );

  // Program-wide settings.
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance(false) );
  const std::string vendor  ( registry["program"].get ( "CadKit" ) );
  const std::string url     ( registry["url"].get     ( ""       ) );
  const std::string program ( registry["program"].get ( "Helios" ) );
  QCoreApplication::setOrganizationName   ( vendor.c_str()  );
  QCoreApplication::setOrganizationDomain ( url.c_str()     );
  QCoreApplication::setApplicationName    ( program.c_str() );

  // Enable toolbar docking.
  this->setEnabled ( true );

  // Make sure we can size the status bar.
  this->statusBar()->setSizeGripEnabled ( true );
  this->statusBar()->showMessage ( tr ( "Ready" ) );

  // Set the title.
  this->setWindowTitle ( program.c_str() );

  // Make sure there is a job-queue for the idle function.
  {
    Usul::Jobs::Queue::RefPtr &queue ( Usul::Jobs::Manager::instance()["idle_queue"] );
    if ( false == queue.valid() )
    {
      queue = Usul::Jobs::Queue::RefPtr ( new Usul::Jobs::Queue ( 0 ) );
    }
  }

  // Make sure there is a job-queue for background tasks.
  {
    Usul::Jobs::Queue::RefPtr &queue ( Usul::Jobs::Manager::instance()["worker_queue"] );
    if ( false == queue.valid() )
    {
      const unsigned int numWorkerThreads ( Usul::Registry::Database::instance()
        ["job_queues"]["worker_queue"]["num_threads"].get<unsigned int> ( 1, true ) );
      queue = Usul::Jobs::Queue::RefPtr ( new Usul::Jobs::Queue ( numWorkerThreads ) );
    }
  }

  // Make sure there is a job-queue for rendering tasks.
  {
    Usul::Jobs::Queue::RefPtr &queue ( Usul::Jobs::Manager::instance()["render_queue"] );
    if ( false == queue.valid() )
    {
      const unsigned int numWorkerThreads ( Usul::Registry::Database::instance()
        ["job_queues"]["render_queue"]["num_threads"].get<unsigned int> ( 1, true ) );
      queue = Usul::Jobs::Queue::RefPtr ( new Usul::Jobs::Queue ( numWorkerThreads ) );
    }
  }

  // Start the idle timer.
  _idleTimer = new QTimer ( this );
  QObject::connect ( _idleTimer, SIGNAL ( timeout() ), SLOT ( _idleProcess() ) );
  _idleTimer->start ( Usul::Registry::Database::instance()["idle_timer"]["milliseconds"].get<int> ( 100, true ) );

  // For drag-and-drop.
  this->setAcceptDrops ( true );

  // Register us as a plugin.
  Usul::Plugins::Manager::instance().add ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );

  // Listen for when the active-view changes.
  Usul::Documents::Manager::instance().activeViewListenerAdd ( IUnknown::QueryPtr ( this ) );

  // Load the persistent settings.
  Usul::Functions::noThrow ( boost::bind ( &MainWindow::_settingsLoad, this ) );

  // Add standard menu buttons.
  MenuBar::RefPtr menuBar ( this->getMenuBar() );
  {
    Menu::RefPtr menu ( new Menu ( "&File" ) );
    menu->append ( new Button ( "&New...", "document_new.png", "Create a new document",
      boost::bind ( &MainWindow::_documentNew, this ),
      boost::bind ( &MainWindow::_enableIfHasPlugin, this, static_cast<unsigned long> ( Usul::Interfaces::IDocumentCreate::IID ), _1 ) ) );
    menu->append ( new Button ( "&Open...", "document_open.png", "Open an existing document",
      boost::bind ( &MainWindow::_documentOpen, this ),
      boost::bind ( &MainWindow::_enableIfHasPlugin, this, static_cast<unsigned long> ( Usul::Interfaces::IDocumentCreate::IID ), _1 ) ) );
    menu->append ( new Button ( "&Save", "document_save.png", "Save the active document",
      boost::bind ( &MainWindow::_documentSave, this ),
      boost::bind ( &MainWindow::_updateDocumentSave, this, _1 ) ) );
    menu->append ( new Button ( "E&xit", "", "Exit the program",
      boost::bind ( &MainWindow::close, this ) ) );
    menuBar->append ( menu );
  }

  // Add standard tool-bar buttons.
  ToolBar::RefPtr toolBar ( this->getToolBar ( "File" ) );
  {
    toolBar->append ( new Button ( "&New...", "document_new.png", "Create a new document",
      boost::bind ( &MainWindow::_documentNew, this ),
      boost::bind ( &MainWindow::_enableIfHasPlugin, this, static_cast<unsigned long> ( Usul::Interfaces::IDocumentCreate::IID ), _1 ) ) );
    toolBar->append ( new Button ( "Open", "document_open.png", "Open an existing document",
      boost::bind ( &MainWindow::_documentOpen, this ),
      boost::bind ( &MainWindow::_enableIfHasPlugin, this, static_cast<unsigned long> ( Usul::Interfaces::IDocumentCreate::IID ), _1 ) ) );
    toolBar->append ( new Button ( "&Save", "document_save.png", "Save the active document",
      boost::bind ( &MainWindow::_documentSave, this ),
      boost::bind ( &MainWindow::_updateDocumentSave, this, _1 ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  Usul::Functions::noThrow ( boost::bind ( &MainWindow::_settingsSave, this ), "3245065420" );
  Usul::Functions::noThrow ( boost::bind ( &MainWindow::_destroy,      this ), "1934297230" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_destroy()
{
  this->threadCheck().throwIfDifferentThread();

  // Stop listening for active-view changes.
  {
    IUnknown::NoDeleteQueryPtr hold ( this );
    {
      Usul::Documents::Manager::instance().activeViewListenerRemove
        ( IUnknown::RefPtr ( hold ) );
    }
  }

  // No more main window.
  Application::instance()._setMainWindow ( 0x0 );

  // Stop the idle timer.
  if ( 0x0 != _idleTimer )
  {
    _idleTimer->stop();
    _idleTimer = 0x0;
  }

  // Clear the menus.
  _menuBar = MenuBarData();
  _toolBars.clear();

  // Delete our job-queues.
  Usul::Jobs::Manager::instance()["idle_queue"]   = Usul::Jobs::Queue::RefPtr ( 0x0 );
  Usul::Jobs::Manager::instance()["worker_queue"] = Usul::Jobs::Queue::RefPtr ( 0x0 );
  Usul::Jobs::Manager::instance()["render_queue"] = Usul::Jobs::Queue::RefPtr ( 0x0 );

  // Purge all documents.
  Usul::Documents::Manager::instance().purge();

  // Release the plugins. Do this before we delete the document manager 
  // because some of the plugins are active document and view listeners.
  Usul::Plugins::Manager::instance().removeAll ( &std::cout );

  // Make sure there are no documents.
  Usul::Documents::Manager::destroy();

  // Unload the libraries. This is a separate step because some of the 
  // plugins do not come from libraries (this class, for example).
  Usul::Plugins::Manager::instance().unloadAll ( &std::cout );

  // Delete all job queues.
  Usul::Jobs::Manager::destroy();

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MainWindow::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the idle timer.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_idleProcess()
{
  typedef boost::posix_time::time_duration Duration;

  USUL_TRY_BLOCK
  {
    this->_updateMenuBar();
    this->_updateToolBars();

    // Execute the next job on the queue.
    Usul::Jobs::Queue::RefPtr queue ( Usul::Jobs::Manager::instance()["idle_queue"] );
    if ( true == queue.valid() )
    {
      // Execute as many jobs as we can fit into our time budget.
      const boost::system_time start ( boost::get_system_time() );
      while ( true == queue->executeNextJob() )
      {
        const Duration d ( boost::get_system_time() - start );
        if ( d > boost::posix_time::milliseconds ( 1000 ) )
        {
          break;
        }
      }
    }
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3734789250" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dereference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::unref ( bool )
{
  unsigned long previous ( _refCount.fetch_and_decrement() );
  if ( 0 == previous )
  {
    throw std::underflow_error ( "Error 2922695833: Decremented a reference count that was already zero" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name of the settings file.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::settingsFile() const
{
  // Get the name of the program.
  const std::string program ( Usul::Registry::Database::instance(false)["program"].get 
    ( boost::filesystem::basename ( Usul::CommandLine::Arguments::instance().get ( 0 ) ), true ) );

  // Make the file name.
  const std::string settings 
    ( Usul::User::Directory::program() + "/" + program + "_settings.rvho" );
  return settings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window is asked to close.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent ( QCloseEvent *event )
{
  this->threadCheck().throwIfDifferentThread();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Determine plugins file.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::pluginsFile() const
{
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance ( false ) );

  // Get executable's path.
  const boost::filesystem::path executable ( Usul::CommandLine::Arguments::instance().get ( 0 ) );

  // The name will either be in the registry or we use the executable's base name.
  std::string file ( registry["plugins"].get ( "", false ) );
  if ( true == file.empty() )
    file = executable.filename();

  // Try the file as is.
  {
    if ( true == boost::filesystem::exists ( file ) )
      return file;
  }

  // Try the user's directory.
  {
    const std::string dir ( Usul::User::Directory::program() );
    const std::string path ( dir + "/" + file );
    if ( true == boost::filesystem::exists ( path ) )
      return path;
  }

  // Try a relative directory.
  {
    const std::string dir ( executable.parent_path().string() + "/../configs" );
    const std::string path ( dir + "/" + file );
    if ( true == boost::filesystem::exists ( path ) )
      return path;
  }

  // We did not find the file.
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::pluginsLoad()
{
  USUL_TRY_BLOCK
  {
    // Get the file.
    const std::string file ( this->pluginsFile() );
    if ( true == file.empty() )
    {
      std::cout << "Failed to find plugin file\n" << std::flush;
      return;
    }
    if ( false == boost::filesystem::exists ( file ) )
    {
      std::cout << Usul::Strings::format ( "Plugin file '", file, "' does not exist", '\n' ) << std::flush;
      return;
    }

    // Get the directory where the program is.
    const std::string dir ( boost::filesystem::path 
      ( Usul::CommandLine::Arguments::instance().get ( 0 ) ).parent_path().string() );

    // Needed below.
    Usul::Plugins::Manager *manager ( &Usul::Plugins::Manager::instance() );
    #ifdef _DEBUG
    const std::string suffix ( "d" );
    #else
    const std::string suffix ( "" );
    #endif

    // Loop through the file.
    typedef Usul::File::InputFileIterator < Usul::File::AsciiInputFile, std::string > Iterator;
    Iterator begin ( new Usul::File::AsciiInputFile ( file ) ), end ( 0x0 );
    for ( Iterator i = begin; i != end; ++i )
    {
      // Get the file name.
      std::string plugin ( ( *i ) + suffix );

      // Prepend the program's directory if needed.
      if ( false == boost::filesystem::exists ( plugin ) )
        plugin = dir + "/" + plugin;

      // Add the plugin if it exists.
      if ( true == boost::filesystem::exists ( plugin ) )
      {
        Usul::Functions::noThrow ( boost::bind 
          ( &Usul::Plugins::Manager::load, manager, plugin ), "1712490900" );
      }
    }
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3643550089" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::pluginsPrint ( std::ostream &out ) const
{
  Usul::Plugins::Manager::instance().print ( out );
}


///////////////////////////////////////////////////////////////////////////////
//
//  If the given file is an open document, bring it forward.
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::_forwardExistingFile ( const std::string &file )
{
  typedef Usul::Interfaces::IUnknown IUnknown;
  Usul::Documents::Manager &dm ( Usul::Documents::Manager::instance() );
  IUnknown::RefPtr existing ( dm.find ( file ) );
  if ( false == existing.valid() )
    return false;

  typedef Usul::Interfaces::IDocumentGet IDocumentGet;
  typedef Usul::Documents::Document Document;
  IDocumentGet::QueryPtr getDoc ( existing );
  Document::RefPtr ed ( getDoc.valid() ? getDoc->documentGet() : 0x0 );
  if ( false == ed.valid() )
    return false;

  ed->viewsForward();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_documentOpen ( const std::string &file )
{
  USUL_TRY_BLOCK
  {
    typedef Usul::Interfaces::IUnknown IUnknown;
    typedef Usul::Interfaces::IRead IRead;
    typedef Usul::Interfaces::IFileDocument IFileDocument;
    typedef Usul::Documents::FileDocument FileDocument;

    // Do not open files that are already open.
    if ( true == this->_forwardExistingFile ( file ) )
      return;

    // Feedback.
    std::cout << Usul::Strings::format ( "Opening document: ", file, '\n' ) << std::flush;

    // Create the document.
    Usul::Documents::Manager &dm ( Usul::Documents::Manager::instance() );
    IUnknown::QueryPtr me ( this );
    IRead::QueryPtr reader ( dm.create ( file, me ).get() );
    if ( false == reader.valid() )
    {
      std::cout << Usul::Strings::format ( "Error 1961361963: Failed to create document for file: ", file ) << std::flush;
      return;
    }

    // Tell it to read the file.
    reader->read ( file, me );

    // Since that worked assign the new file name.
    IFileDocument::QueryPtr idoc ( reader );
    FileDocument::RefPtr doc ( ( true == idoc.valid() ) ? idoc->fileDocument() : 0x0 );
    if ( true == doc.valid() )
    {
      typedef Usul::Documents::FileDocument::FileInfo FileInfo;
      FileInfo info ( file, true );
      doc->fileName ( info );
    }

    // This document is no longer modified.
    doc->modified ( false );

    // Add job to create gui.
    Usul::Jobs::Queue::ValidRefPtr queue ( Usul::Jobs::Manager::instance()["idle_queue"] );
    queue->add ( boost::bind ( &MainWindow::_createGui, this, IUnknown::QueryPtr ( doc ) ) );

    // Feedback.
    std::cout << Usul::Strings::format ( "Done opening document: ", file, '\n' ) << std::flush;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2991944830" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new document.
//  TODO: prompt user to choose between different types.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_documentNew()
{
  USUL_TRY_BLOCK
  {
    typedef Usul::Interfaces::IUnknown IUnknown;
    typedef Usul::Interfaces::IDocumentCreate IDocumentCreate;

    // Find the first creator.
    Usul::Plugins::Manager &pm ( Usul::Plugins::Manager::instance() );
    IDocumentCreate::QueryPtr creator ( pm.findFirst ( IDocumentCreate::IID ).get() );
    if ( false == creator.valid() )
      return;

    // Create the document.
    IUnknown::RefPtr doc ( creator->createDocument ( IUnknown::QueryPtr ( this ) ) );
    if ( false == doc.valid() )
    {
      std::cout << "Error 2711186213: Failed to create new document \n" << std::flush;
      return;
    }

    // Add job to create gui.
    Usul::Jobs::Queue::ValidRefPtr queue ( Usul::Jobs::Manager::instance()["idle_queue"] );
    queue->add ( boost::bind ( &MainWindow::_createGui, this, doc ) );
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1179648960" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save active document.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_documentSave()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_createGui ( Usul::Interfaces::IUnknown::RefPtr )
{
  this->threadCheck().throwIfDifferentThread();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the menu bar. Make it if you have to.
//
///////////////////////////////////////////////////////////////////////////////

Helios::Menus::MenuBar::RefPtr MainWindow::getMenuBar()
{
  AtomicMenuBar::Guard guard ( _menuBar.mutex() );
  MenuBarData &data ( _menuBar.getReference() );
  Menus::MenuBar::RefPtr &mb ( data.get<0>() );
  if ( false == mb.valid() )
  {
    mb = Menus::MenuBar::RefPtr ( new Menus::MenuBar );
  }
  return mb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the tool bar. Make it if you have to.
//
///////////////////////////////////////////////////////////////////////////////

Helios::Menus::ToolBar::RefPtr MainWindow::getToolBar ( const std::string &name )
{
  if ( true == name.empty() )
    throw std::invalid_argument ( "Error 4019783738: Requesting toolbar using empty name" );

  AtomicToolBarMap::Guard guard ( _toolBars.mutex() );
  ToolBarMap &tbm ( _toolBars.getReference() );
  ToolBarData &data ( tbm[name] );
  Menus::ToolBar::RefPtr &tb ( data.get<0>() );
  if ( false == tb.valid() )
  {
    tb = Menus::ToolBar::RefPtr ( new Menus::ToolBar ( name ) );
  }

  // Make it visible.
  data.get<3>() = true;

  return tb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the toolbars.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_updateToolBars()
{
  AtomicToolBarMap::Guard guard ( _toolBars.mutex() );
  ToolBarMap &tbm ( _toolBars.getReference() );
  for ( ToolBarMap::iterator i = tbm.begin(); i != tbm.end(); ++i )
  {
    this->_updateToolBar ( i->second );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the toolbar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_updateToolBar ( ToolBarData &tbd )
{
  this->threadCheck().throwIfDifferentThread();

  // Punt if no toolbar.
  Menus::ToolBar::RefPtr &tb ( tbd.get<0>() );
  if ( false == tb.valid() )
    return;

  // Punt if there is nothing to do.
  if ( ( false == tb->needToUpdate() ) && ( false == tb->needToRebuild() ) )
    return;

  // Get the QToolBar.
  QToolBarPtr &qtb ( tbd.get<1>() );

  // If the toolbar is empty then we remove it (deleting it is unstable).
  if ( true == tb->empty() )
  {
    if ( 0x0 != qtb.get() )
    {
      this->removeToolBar ( qtb.get() );
      tb->needToRebuild ( false );
      tb->needToUpdate ( false );
    }
    return;
  }

  // When we get to here make sure we have a toolbar.
  if ( 0x0 == qtb.get() )
  {
    const QString name ( tb->name().c_str() );
    qtb = QToolBarPtr ( this->addToolBar ( name ) );
    qtb->setObjectName ( name );
  }

  // Should the toolbar be visible?
  const bool visible ( tbd.get<3>() );
  if ( true == visible )
  {
    qtb->show();
  }
  else
  {
    qtb->hide();
  }

  // The first time we make the visitor.
  Menus::Builder::RefPtr &tbv ( tbd.get<2>() );
  if ( false == tbv.valid() )
  {
    tbv = Menus::Builder::RefPtr ( new Menus::Builder ( qtb.get() ) );
  }

  // Do we need to rebuild?
  if ( true == tb->needToRebuild() )
  {
    // Build the toolbar.
    tbv->reset();
    tb->accept ( *tbv );
    tb->needToRebuild ( false );
  }

  // Do we need to update?
  if ( true == tb->needToUpdate() )
  {
    tbv->updateActions();
    tb->needToUpdate ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menubar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_updateMenuBar()
{
  this->threadCheck().throwIfDifferentThread();

  // Lock and get reference.
  AtomicMenuBar::Guard guard ( _menuBar.mutex() );
  MenuBarData &mbd ( _menuBar.getReference() );

  // Punt if no menubar, or if it's not dirty.
  Menus::MenuBar::RefPtr &mb ( mbd.get<0>() );
  if ( false == mb.valid() )
    return;
  if ( false == mb->isDirty() )
    return;

  // The first time we make the visitor.
  Menus::Builder::RefPtr &mbv ( mbd.get<1>() );
  if ( false == mbv.valid() )
  {
    mbv = Menus::Builder::RefPtr ( new Menus::Builder ( this->menuBar() ) );
  }

  // Purge empty menus.
  mb->purge();

  // Build the menu.
  mbv->reset();
  mb->accept ( *mbv );

  // No longer dirty.
  mb->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the action if there are plugins with the interface.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_enableIfHasPlugin ( unsigned long iid, QAction &action ) const
{
  this->threadCheck().throwIfDifferentThread();
  typedef Usul::Interfaces::IUnknown::RefPtr UnknownPtr;
  Usul::Plugins::Manager &pm ( Usul::Plugins::Manager::instance() );
  UnknownPtr unknown ( pm.findFirst ( iid ) );
  action.setEnabled ( true == unknown.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the action if the active document needs to be saved.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_updateDocumentSave ( QAction &action ) const
{
  this->threadCheck().throwIfDifferentThread();
  Usul::Interfaces::IDocumentGet::QueryPtr getter 
    ( Usul::Documents::Manager::instance().activeDocumentGet().get() );
  Usul::Documents::Document::RefPtr active 
    ( ( true == getter.valid() ) ? getter->documentGet() : 0x0 );
  action.setEnabled ( ( true == active.valid() ) ? active->isModified() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the thread-checking object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Check MainWindow::threadCheck() const
{
  return Application::instance().threadCheck();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the active view changes.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::activeViewChanged ( IUnknown::RefPtr, IUnknown::RefPtr )
{
  // All the toolbars need to update their buttons.
  this->_needToUpdateToolBars ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flag all the toolbars.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_needToUpdateToolBars ( bool state )
{
  AtomicToolBarMap::Guard ( _toolBars.mutex() );
  ToolBarMap &tbm ( _toolBars.getReference() );
  for ( ToolBarMap::iterator i = tbm.begin(); i != tbm.end(); ++i )
  {
    ToolBarData &data ( i->second );
    Menus::ToolBar::RefPtr &tb ( data.get<0>() );
    if ( true == tb.valid() )
    {
      tb->needToUpdate ( state );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show this window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::show()
{
  this->threadCheck().throwIfDifferentThread();

  this->_restoreDockWindows();
  this->_updateMenuBar();
  this->_updateToolBars();

  BaseClass::show();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this window. This gets called after the plugins are loaded.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::init()
{
  this->threadCheck().throwIfDifferentThread();

  // Add menu for dockable windows.
  this->_initDockableWindowMenu();

  // Create a new document if we should.
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance(false) );
  const bool create ( registry["create_new_document"].get<bool> ( false, false ) );
  if ( true == create )
  {
    this->_documentNew();
  }

  // Load files from the command-line.
  typedef Usul::CommandLine::Arguments Arguments;
  const unsigned int argc ( Arguments::instance().size() );
  if ( argc > 0 )
  {
    for ( unsigned int i = 1; i < argc; ++i )
    {
      const std::string arg ( Arguments::instance().get ( i ) );
      if ( true == boost::filesystem::exists ( arg ) )
      {
        Usul::Jobs::Queue::ValidRefPtr queue 
          ( Usul::Jobs::Manager::instance()["worker_queue"] );
        queue->add ( boost::bind ( &MainWindow::_documentOpen, this, arg ) );

        // Stop now if we're sdi.
        if ( MainWindow::SINGLE_DOCUMENT == this->type() )
        {
          i = argc;
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the menu for dockable windows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initDockableWindowMenu()
{
  this->threadCheck().throwIfDifferentThread();

  using namespace Helios::Menus;
  const std::string menuName ( "Docking Windows" );

  // If we find the menu then remove it.
  MenuBar::RefPtr menuBar ( this->getMenuBar() );
  {
    Menu::RefPtr view ( menuBar->at ( menuBar->find ( "&View" ) ) );
    if ( view.valid() )
    {
      view->remove ( menuName );
      menuBar->dirty ( true );
    }
  }

  // Get all the dockable windows.
  typedef QList<QDockWidget*> DockWidgets;
  DockWidgets dockable ( this->findChildren<QDockWidget*>() );
  if ( true == dockable.empty() )
    return;

  // Get or make the view menu.
  Menu::RefPtr view ( menuBar->menu ( "&View", menuBar->find ( "&File" ) + 1 ) );
  Menu::RefPtr menu ( view->menu ( menuName, 0 ) );

  // Used if the window does not have a name.
  unsigned int count ( 0 );

  // Loop through the windows.
  for ( DockWidgets::iterator i = dockable.begin(); i != dockable.end(); ++i )
  {
    // Get the window.
    QDockWidget *window ( *i );
    if ( 0x0 != window )
    {
      // Get the window name. Make one if empty.
      std::string name ( window->windowTitle().toAscii() );
      if ( true == name.empty() )
      {
        name = Usul::Strings::format ( "Dock Window ", ++count );
      }

      // Append a button to toggle this window.
      menu->append
      (
        new Button
        (
          name, "", Usul::Strings::format ( "Toggle ", name, " visibility" ),
          boost::bind
          (
            &QDockWidget::setVisible, window, 
            !boost::bind ( &QDockWidget::isVisible, window )
          ),
          Functors::updateCheckState
          (
            boost::bind ( &QDockWidget::isVisible, window )
          )
        )
      );
    }
  }

  // The separator is visible when additional buttons are added.
  menu->append ( Separator::RefPtr ( new Separator ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_settingsLoad()
{
  this->threadCheck().throwIfDifferentThread();

  // Shortcut to registry.
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance() );

  // Handle no settings file.
  const std::string file ( this->settingsFile() );
  if ( false == boost::filesystem::exists ( file ) )
    return;

  // Read the file and populate the registry. Not a big deal if this fails.
  Usul::Functions::noThrow ( boost::bind 
    ( Tree::Registry::IO::read, file, boost::ref ( registry ) ), "1123442106" );

  // Set the window's properties.
  Usul::Registry::Node &mw ( registry["main_window"] );
  this->restoreGeometry ( mw["geometry"].get<QByteArray> ( QByteArray() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_settingsSave()
{
  this->threadCheck().throwIfDifferentThread();

  // Shortcut to registry.
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance() );

  // Set the window's properties.
  Usul::Registry::Node &mw ( registry["main_window"] );
  mw["geometry"] = this->saveGeometry();

  // Save dock window positions.
  mw["dock_window_positions"] = this->saveState();

  // Write to disk.
  const std::string file ( this->settingsFile() );
  Usul::Functions::noThrow ( boost::bind 
    ( Tree::Registry::IO::write, file, boost::ref ( registry ) ), "4136994389" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore dock window positions.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_restoreDockWindows()
{
  this->threadCheck().throwIfDifferentThread();

  // Shortcut to registry.
  Usul::Registry::Database &registry ( Usul::Registry::Database::instance() );
  Usul::Registry::Node &section ( registry["main_window"]["dock_window_positions"] );

  // Get positions from registry.
  QByteArray positions ( section.get<QByteArray> ( QByteArray() ) );

  // Only restore positions if we have some.
  if ( positions.size() > 0 )
  {
    this->restoreState ( positions );
  }
}
