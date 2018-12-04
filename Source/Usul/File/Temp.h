
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Temporary file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_TEMPORARY_H_
#define _USUL_FILE_TEMPORARY_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace File {


struct USUL_EXPORT Temp
{
  // Return name of directory where temporary files can be created.
  static std::string          directory();

  // Return a temporary file name.
  static std::string          file();
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_TEMPORARY_H_
