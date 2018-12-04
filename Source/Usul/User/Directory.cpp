
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User's directories.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/User/Directory.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Registry/Database.h"
#include "Usul/System/Environment.h"

#include "boost/filesystem.hpp"

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <shlobj.h>
#endif

#include <algorithm>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's document directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::documents()
{
#ifdef _MSC_VER

  std::string dir;
  char path [ MAX_PATH * 2 ];
  if ( FALSE == SHGetSpecialFolderPath ( 0x0, path, CSIDL_PERSONAL, TRUE ) )
  {
    throw std::runtime_error ( "Error 2904267615: Failed to find documents directory" );
  }
  dir = path;

#else

  // Look for environment variable.
  std::string dir ( Usul::System::Environment::get ( "HOME" ) );

#endif

  // Punt if the string is empty.
  if ( true == dir.empty() )
  {
    throw std::runtime_error ( "Error 2322383190: Failed to find user's document directory" );
  }

#ifdef _MSC_VER

  // Make all the slashes forward.
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

#endif

  // No trailing slash.
  if ( '/' == dir.at ( dir.size() - 1 ) )
  {
    dir.erase ( dir.size() - 1 );
  }

  // Return string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's application-data directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::appData()
{
#ifdef _MSC_VER

  std::string dir;
  char path [ MAX_PATH * 2 ];
  if ( FALSE == SHGetSpecialFolderPath ( 0x0, path, CSIDL_APPDATA, TRUE ) )
  {
    throw std::runtime_error ( "Error 2792381829: Failed to find application-data directory" );
  }
  dir = path;

#else

  // Look for environment variable on non-windows systems.
  std::string dir ( Usul::System::Environment::get ( "HOME" ) );

#endif

  // Punt if the string is empty.
  if ( true == dir.empty() )
  {
    throw std::runtime_error ( "Error 3888703160: Failed to find user's application-data directory" );
  }

#ifdef _MSC_VER

  // Make all the slashes forward.
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

#endif

  // No trailing slash.
  if ( '/' == dir.at ( dir.size() - 1 ) )
  {
    dir.erase ( dir.size() - 1 );
  }

  // Return string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's vendor directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::vendor ( const std::string &vendor )
{
  // Handle bad input.
  if ( vendor.empty() )
  {
    throw std::runtime_error ( "Error 2017220875: Empty vendor name given." );
  }

  // Get the application-data directory with the slash.
  const std::string dir ( Usul::User::Directory::appData() );

  // Make the path. Note: if this is not Windows then we follow the unix 
  // convention of making a "dot" directory.
#ifdef _MSC_VER
  const std::string path ( dir + "/" + vendor );
#else
  const std::string path ( dir + "/." + vendor );
#endif

  // Return string.
  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's program directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::program ( const std::string &vendor, const std::string &program )
{
  // Handle bad input.
  if ( program.empty() )
  {
    throw std::runtime_error ( "Error 3166677210: Empty program name given." );
  }

  // Get the vendor directory.
  const std::string dir ( Usul::User::Directory::vendor ( vendor ) );

  // Make the path.
  const std::string path ( dir + "/" + program );

  // Return string.
  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get a valid string as the name of the program.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  std::string getProgramName()
  {
    // First try the registry.
    std::string name ( Usul::Registry::Database::instance ( false )["program"].get ( "", false ) );
    if ( true == name.empty() )
    {
      // Try to make the name from the command-line.
      name = boost::filesystem::basename ( Usul::CommandLine::Arguments::instance().get ( 0 ) );
      if ( true == name.empty() )
      {
        name = "DefaultProgramName";
      }
    }
    return name;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's program directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::program()
{
  const std::string v ( Usul::Registry::Database::instance ( false )["vendor"].get ( "DefaultVendorName",  false ) );
  const std::string p ( Helper::getProgramName() );
  return Usul::User::Directory::program ( v, p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's vendor directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::vendor()
{
  const std::string v ( Usul::Registry::Database::instance ( false )["vendor"].get ( "DefaultVendorName", false ) );
  return Usul::User::Directory::vendor ( v );
}
