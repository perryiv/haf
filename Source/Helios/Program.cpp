
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
//  Helios program class. Handles startup, shutdown, and branding.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Program.h"
#include "Helios/Application.h"
#include "Helios/MainWindows/MainWindowMDI.h"
#include "Helios/MainWindows/MainWindowSDI.h"
#include "Helios/OutputFile.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

#include "QtCore/QCoreApplication"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/filesystem.hpp"

#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace Helios;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to print output from Qt.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void qtOutput ( QtMsgType t, const char *m )
  {
    USUL_TRY_BLOCK
    {
      if ( 0x0 == m )
        return;

      // Drop multiple trailing newlines.
      std::string s ( m );
      std::replace ( s.begin(), s.end(), '\r', '\n' );
      while ( ( false == s.empty() ) && ( '\n' == s.at ( s.size() - 1 ) ) )
        s.erase ( s.size() - 1 );

      switch ( t )
      {
        case QtDebugMsg:
          std::cout << Usul::Strings::format ( "Qt Debug Message: ", s, '\n' ) << std::flush;
          break;
        case QtWarningMsg:
          std::cout << Usul::Strings::format ( "Qt Warning Message: ", s, '\n' ) << std::flush;
          break;
        case QtCriticalMsg:
          std::cout << Usul::Strings::format ( "Qt Critical Message: ", s, '\n' ) << std::flush;
          break;
        case QtFatalMsg:
          std::cout << Usul::Strings::format ( "Qt Fatal Message: ", s, '\n' ) << std::flush;
          break;
      }
    }
    catch ( ... )
    {
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to update registy values with command-line options.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void updateRegistry()
  {
    typedef Usul::CommandLine::Arguments::Strings Args;
    typedef std::vector<std::string> Parts;

    // Used below.
    const std::string::value_type delimiter ( '=' );
    Usul::Registry::Database &registry ( Usul::Registry::Database::instance ( false ) );

    // Get command-line arguments and loop through them.
    const Args args ( Usul::CommandLine::Arguments::instance().get() );
    for ( Args::const_iterator i = args.begin(); i != args.end(); ++i )
    {
      // Split the argument.
      const std::string arg ( *i );
      Parts parts;
      boost::algorithm::split ( parts, arg, std::bind1st ( std::equal_to<char>(), delimiter ) );

      // If there are two parts...
      if ( 2 == parts.size() )
      {
        const std::string name ( parts.front() );
        const std::string value ( parts.back() );

        // If the name is not empty...
        if ( false == name.empty() )
        {
          // Set the value in the registry.
          registry[name] = value;
        }
      }
    }
  }
}


  

///////////////////////////////////////////////////////////////////////////////
//
//  Format date string. Pass __DATE__ macro.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline std::string formatDate ( const std::string &d )
  {
    // If the string is not this size then the logic below does not work.
    if ( 11 != d.size() )
      return d;

    const std::string year  ( d.begin() + 7, d.end() );
    const std::string month ( d.begin() + 0, d.begin() + 3 );
    const std::string day   ( d.begin() + 4, d.begin() + 6 );

    std::ostringstream out;
    out << day << '-' << month << '-' << year;

    return out.str();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make the main-window and run the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class MainWindow, class OutputFile >
  void run ( OutputFile &output )
  {
    MainWindow mw;

    // Some helpful information.
    std::cout << "Text output: " << output.file() << std::endl;
    std::cout << "Settings file: " << mw.settingsFile() << std::endl;

    // Load the plugins.
    mw.pluginsLoad();
    mw.pluginsPrint ( std::cout );

    // Initialize it. This has to happen after loading plugins.
    mw.init();

    // Show it.
    mw.show();

    // Run the main event loop.
    Application::instance().exec();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run ( int argc, char **argv )
{
  // Synchronize std::cout and printf.
  std::ios::sync_with_stdio();

  // Capture Qt output.
  ::qInstallMsgHandler ( &Helper::qtOutput );

  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Update the registry from the command-line arguments.
  Helper::updateRegistry();

  // Make sure the program's directory exists.
  boost::filesystem::create_directories ( boost::filesystem::path ( Usul::User::Directory::program() ) );

  // Get the output file name.
  const OutputFile output;

  // Nested scopes below to control order of deletion.
  {
    // Redirect stdout and stderr to a file.
    Usul::IO::Redirect redirect ( output.file() );
    {
      Usul::Registry::Database &registry ( Usul::Registry::Database::instance ( false ) );
      std::cout << "Welcome to " << registry["program"].get ( "DefaultProgram" ) << ' ' << registry["version"].get ( "Version" ) << std::endl;
      std::cout << "Built on " << Helper::formatDate ( __DATE__ ) << " at " << __TIME__ << std::endl;

      Helios::Application::Life app ( argc, argv );
      {
        const std::string type ( registry["app_type"].get ( "mdi" ) );
        if ( "mdi" == boost::algorithm::to_lower_copy ( type ) )
        {
          typedef Helios::MainWindows::MainWindowMDI MainWindow;
          Helper::run<MainWindow> ( output );
        }
        else
        {
          typedef Helios::MainWindows::MainWindowSDI MainWindow;
          Helper::run<MainWindow> ( output );
        }
      }
    }
  }
}
