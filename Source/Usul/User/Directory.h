
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User's directories.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_USER_DIRECTORY_CLASS_H_
#define _USUL_USER_DIRECTORY_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace User {


struct USUL_EXPORT Directory
{
  // Get the application-data directory.
  static std::string      appData();

  // Get user's documents directory. If environment variable 'HOME' is set, then that is 
  // returned. On Windows, if there is no 'HOME' environment variable, then 
  // the 'My Documents' directory in the user's profile is returned. 
  static std::string      documents();

  // Get the directory in which the vendor can store persistent data. Throws if it fails.
  static std::string      vendor ( const std::string &vendorName );
  static std::string      vendor();

  // Get the directory in which the program can store persistent data. Throws if it fails.
  static std::string      program ( const std::string &vendorName, const std::string &programName );
  static std::string      program();
};


} // namespace User
} // namespace Usul


#endif // _USUL_USER_DIRECTORY_CLASS_H_
