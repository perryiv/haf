
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios program class. Handles startup, shutdown, and branding.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_OUTPUT_FILE_H_
#define _HELIOS_OUTPUT_FILE_H_

#include "Usul/Config/Config.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/System/Process.h"
#include "Usul/User/Directory.h"

#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Class to help with managing the output file. If the program is already 
//  running then we want the output to go to a file with some random name in 
//  the temporary directory, and we remove it when done. Otherwise, we use 
//  the perferred name and do not remove it.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helios
{
  struct OutputFile
  {
    OutputFile() : _file(), _remove ( false )
    {
      // Get the name of the executable.
      const std::string executable ( boost::filesystem::basename ( Usul::CommandLine::Arguments::instance().get ( 0 ) ) );

      // Count the number of processes with this program's name.
      // Sum the number of both release or debug executables.
      #ifdef _DEBUG
      const std::string release ( executable.begin(), executable.begin() + executable.size() - 1 );
      const unsigned int count ( Usul::System::Process::count ( executable ) + Usul::System::Process::count ( release ) );
      #else
      const std::string debug ( executable + "d" );
      const unsigned int count ( Usul::System::Process::count ( executable ) + Usul::System::Process::count ( debug ) );
      #endif

      // If this program is already running, in either debug or release 
      // mode, then the count should be greater than one.
      if ( count > 1 )
      {
        _file = Usul::File::Temp::file();
        _remove = true;
      }

      // Otherwise, this is the only instance so use the preferred name.
      else
      {
        const std::string program ( Usul::Registry::Database::instance(false)["program"].get ( executable, true ) );
        _file = ( Usul::User::Directory::program() + "/" + program + ".stdout" );
        { Usul::Scope::RemoveFile ( _file, false ); }
        _remove = false;
      }

      // Register this file name.
      Usul::Registry::Database::instance(false)["text_output_file"] = _file;
    }
    ~OutputFile()
    {
      if ( true == _remove )
      {
        { Usul::Scope::RemoveFile ( _file, false ); }
      }
    }
    std::string file() const
    {
      return _file;
    }
  private:
    OutputFile ( const OutputFile &f );
    OutputFile &operator = ( const OutputFile & );
    std::string _file;
    mutable bool _remove;
  };
}


#endif // _HELIOS_OUTPUT_FILE_H_
