
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/TextOutputWindow/TestOutputPlugin.h"
#include "Helios/Application.h"
#include "Helios/MainWindows/MainWindow.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/Plugins/Helper.h"
#include "Usul/System/Sleep.h"

#include "QtGui/QDockWidget"
#include "QtGui/QTextEdit"

#include "boost/bind.hpp"
#include "boost/filesystem/operations.hpp"

#include <fstream>
#include <vector>

using namespace Helios::Plugins::TextOutputWindow;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TestOutputPlugin, BaseClass );
USUL_PLUGIN_INITIALIZE ( TestOutputPlugin );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TestOutputPlugin::TestOutputPlugin() : BaseClass(),
  _dock ( 0x0 ),
  _text ( 0x0 ),
  _offset ( 0 ),
  _lines(),
  _file(),
  _jobs(),
  _threadCheck()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TestOutputPlugin::~TestOutputPlugin()
{
  Usul::Functions::noThrow ( boost::bind ( &TestOutputPlugin::_destroy, this ), "2017102918" );
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
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::_destroy()
{
  // Clear most member.
  this->_clear();

  // Get shortcut to job manager.
  Usul::Jobs::Manager &jm ( Usul::Jobs::Manager::instance() );

  // Reset the jobs.
  Jobs jobs ( _jobs.fetchAndStore ( Jobs() ) );

  // Loop through the jobs and remove them from this queue.
  std::for_each ( jobs.begin(), jobs.end(), boost::bind 
    ( Helper::cancelAndWait, jm["text_output_queue"], _1 ) );

  // Loop and remove from other queue. This is redundant but should be safe.
  std::for_each ( jobs.begin(), jobs.end(), boost::bind 
    ( Helper::cancelAndWait, jm["idle_queue"], _1 ) );

  // Delete our queue.
  Usul::Jobs::Manager::instance()["text_output_queue"] = Usul::Jobs::Queue::RefPtr();

  // Don't need these anymore.
  _file.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this instance.
//
///////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::_clear()
{
  // Have to delete these before the library is released.
  if ( 0x0 != _text )
  {
    delete _text;
    _text = 0x0;
  }
  if ( 0x0 != _dock )
  {
    delete _dock;
    _dock = 0x0;
  }

  // Clear the lines.
  _lines.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TestOutputPlugin::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

std::string TestOutputPlugin::pluginDescription() const
{
  return std::string ( 
    "Displays stdout and stderr streams in a dockable, read-only text window" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is loaded.
//
/////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::pluginInitialize()
{
  Guard guard ( this );

  // Handle repeated calls.
  this->_clear();

  // Get the main window.
  MainWindows::MainWindow *mw ( Application::instance().mainWindow() );
  if ( 0x0 == mw )
    return;

  // Build the docking window.
  const std::string name ( "Text Output" );
  _dock = new QDockWidget ( QDockWidget::tr ( name.c_str() ), mw );
  _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  // Set the object name. This is needed to save and restore dock position.
  _dock->setObjectName ( QDockWidget::tr ( name.c_str() ) );

  // Make the text widget.
  _text = new QTextEdit ( _dock );
  _text->setReadOnly ( true );

  // Dock it.
  _dock->setWidget ( _text );
  mw->addDockWidget ( Qt::BottomDockWidgetArea, _dock );

  // Is there a file?
  Usul::Registry::Database &reg ( Usul::Registry::Database::instance ( false ) );
  const std::string file ( reg["text_output_file"].get ( "" ) );
  if ( true == boost::filesystem::exists ( file ) )
  {
    // Save the file.
    _file = file;

    // Add a job to read the file.
    Usul::Jobs::Manager &jm ( Usul::Jobs::Manager::instance() );
    Usul::Jobs::Queue::RefPtr &queue ( jm["text_output_queue"] );
    if ( false == queue.valid() )
    {
      queue = Usul::Jobs::Queue::RefPtr ( new Usul::Jobs::Queue ( 1 ) );
    }
    Guard g2 ( _jobs.mutex() );
    Jobs &jobs ( _jobs.getReference() );
    jobs.insert ( queue->add ( boost::bind 
      ( &TestOutputPlugin::_readOutputFile, this, _1 ) ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is released.
//
/////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::pluginFinalize()
{
  this->_clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called to read the file.
//
/////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::_readOutputFile ( Job::RefPtr job )
{
  // Remove the job.
  {
    Guard guard ( _jobs.mutex() );
    Jobs &jobs ( _jobs.getReference() );
    jobs.erase ( job );
  }

  // Check thread.
  _threadCheck.throwIfSameThread();

  // Make sure it exists.
  const std::string file ( _file );
  if ( false == boost::filesystem::exists ( file ) )
    return;

  // Open the file.
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    return;

  // Skip ahead to the last place you read.
  const unsigned int offset ( _offset );
  in.seekg ( offset );

  // Make the buffer.
  typedef std::vector < char > Buffer;
  Buffer buffer ( 2048, '\0' );
  const unsigned int last ( buffer.size() - 1 );

  // Read to the end of the file.
  while ( EOF != in.peek() )
  {
    // Get the line.
    in.getline ( &buffer[0], last );
    buffer[last] = '\0';
    const std::string line ( &buffer[0] );

    // Append to our list if it is not blank.
    if ( false == line.empty() )
    {
      Guard guard ( _lines.mutex() );
      _lines.getReference().push_back ( line );
    }
  }

  // Save the offset.
  _offset = static_cast < unsigned int > ( in.tellg() );

  // Add a job to populate the text window.
  Usul::Jobs::Manager &jm ( Usul::Jobs::Manager::instance() );
  Usul::Jobs::Queue::RefPtr &queue ( jm["idle_queue"] );
  if ( true == queue.valid() )
  {
    Guard guard ( _jobs.mutex() );
    Jobs &jobs ( _jobs.getReference() );
    jobs.insert ( queue->add ( boost::bind 
      ( &TestOutputPlugin::_appendLines, this, _1 ) ) );
  }

  // Sleep for a while.
  Usul::System::Sleep::milliseconds ( 500 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called to append the lines to the text window.
//
/////////////////////////////////////////////////////////////////////////////

void TestOutputPlugin::_appendLines ( Job::RefPtr job )
{
  // Remove the job.
  {
    Guard guard ( _jobs.mutex() );
    Jobs &jobs ( _jobs.getReference() );
    jobs.erase ( job );
  }

  // Check thread.
  _threadCheck.throwIfDifferentThread();

  // Swap the lines with this local variable.
  StringList lines;
  {
    Guard guard ( _lines.mutex() );
    lines.swap ( _lines.getReference() );
  }

  // Loop through the lines.
  while ( false == lines.empty() )
  {
    // Get the line.
    const std::string line ( lines.front() );
    lines.pop_front();

    // Append the line.
    if ( false == line.empty() )
    {
      Guard guard ( this );
      _text->append ( line.c_str() );
    }
  }

  // Add another job to read the file.
  Usul::Jobs::Manager &jm ( Usul::Jobs::Manager::instance() );
  Usul::Jobs::Queue::RefPtr &queue ( jm["text_output_queue"] );
  if ( true == queue.valid() )
  {
    Guard guard ( _jobs.mutex() );
    Jobs &jobs ( _jobs.getReference() );
    jobs.insert ( queue->add ( boost::bind 
      ( &TestOutputPlugin::_readOutputFile, this, _1 ) ) );
  }
}
