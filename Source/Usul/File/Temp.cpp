
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

#include "Usul/File/Temp.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Clock.h"
#include "Usul/System/LastError.h"
#include "Usul/System/Process.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/MPL/StaticAssert.h"

#include "boost/filesystem.hpp"

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <cstdio>
#include <stdexcept>

using namespace Usul::File;


///////////////////////////////////////////////////////////////////////////////
//
//  Return a valid temporary file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Temp::file()
{
  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

#ifdef _MSC_VER // Visual C++

  // Work-around for Win32's tmpnam() implementation. It does not consider if 
  // the directory is write-protected.

  // Compile-time sanity check.
  const unsigned int bufSize ( 16383 ); // 2^14 - 1
  USUL_STATIC_ASSERT ( bufSize > MAX_PATH );

  // Get the directory where temporary files can be created.
  char directory[bufSize + 1];
  DWORD result ( ::GetTempPath ( bufSize, directory ) );

  // Check for errors.
  if ( 0 == result || bufSize < result )
    throw std::runtime_error ( std::string ( "Error 3143231617: failed get path for temporary files. " ) + LastError::message() );

  // Get the file name.
  char pathname[bufSize + 1];
  result = ::GetTempFileName ( directory, "temp_", 0, pathname );

  // Check for errors.
  if ( 0 == result )
    throw std::runtime_error ( std::string ( "Error 2499845465: failed to create temporary file. " ) + LastError::message() );

  // Return the name.
  return std::string ( pathname );

#else

  return ( Usul::Strings::format (
    std::string ( P_tmpdir ), 
    '/', Usul::System::Process::currentProcessId(),
    '_', Usul::Threads::currentThreadId(),
    '_', Usul::System::Clock::milliseconds(),
    ".tmp" ) );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of directory where temporary files can be created.
//
///////////////////////////////////////////////////////////////////////////////

std::string Temp::directory()
{
#ifdef _MSC_VER

  const boost::filesystem::path p ( Temp::file() );
  return p.parent_path().string();

#else

  return std::string ( P_tmpdir );

#endif
}
