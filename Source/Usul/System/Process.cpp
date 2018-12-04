
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Process classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/System/Process.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem.hpp"

#if _MSC_VER
#include "windows.h"
#include "tlhelp32.h"
#else
#include "Usul/System/ProcessUnix.h"
#endif

#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to close a handle.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  void closeHandle ( HANDLE &h )
  {
    if ( 0x0 != h )
    {
      ::CloseHandle ( h );
      h = 0x0;
    }
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to keep track of data.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  struct Data
  {
    Data() : _pi()
    {
      ::ZeroMemory ( &_pi, sizeof ( PROCESS_INFORMATION ) );
    }
    ~Data()
    {
      Helper::closeHandle ( _pi.hProcess );
      Helper::closeHandle ( _pi.hThread );
    }
    PROCESS_INFORMATION &processInfo()
    {
      return _pi;
    }
  private:
    PROCESS_INFORMATION _pi;
  };
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the process information.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  PROCESS_INFORMATION &getProcessInfo ( void *ptr )
  {
    Helper::Data *data ( reinterpret_cast < Helper::Data * > ( ptr ) );
    if ( 0x0 == data )
    {
      throw std::runtime_error ( "Error 3581526434: Process information in null" );
    }
    return data->processInfo();
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to delete the process information.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  void deleteProcessInfo ( void *ptr )
  {
    Helper::Data *data ( reinterpret_cast < Helper::Data * > ( ptr ) );
    if ( 0x0 != data )
    {
      delete data;
    }
  }
}

#else

namespace Helper
{
  void deleteProcessInfo ( void *ptr )
  {
    Detail::ProcessImpl* impl ( reinterpret_cast< Detail::ProcessImpl*> ( ptr ) );
    if ( 0x0 != impl )
    {
	  delete impl;
    }
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to throw informative exception.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void throwException ( const std::string &message, const std::string &executable, const std::string &arguments, Usul::System::Process::ID id )
  {
    const std::string systemErrorMessage ( ( true == Usul::System::LastError::has() ) ? ( std::string ( ", System error: " ) + Usul::System::LastError::message() ) : std::string() );
    const std::string idString ( ( 0 != id ) ? ( std::string ( ", ID: " ) + Usul::Convert::Type<unsigned int,std::string>::convert ( static_cast<unsigned int> ( id ) ) ) : std::string() );
    throw std::runtime_error ( Usul::Strings::format ( message, ", Executable: ", executable, ", Arguments: ", arguments, idString, systemErrorMessage ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Process::Process ( const std::string &executable, 
                   const std::string &arguments ) : BaseClass(),
  _executable ( executable ),
  _arguments ( arguments ),
  _data ( 0x0 ),
  _hasBeenStopped ( false )
{
  // Need a writable buffer for command-line arguments.
  // http://msdn.microsoft.com/en-us/library/ms682425(VS.85).aspx
  std::vector<char> args ( arguments.begin(), arguments.end() );

  // Make sure there is a space before the first argument.
  if ( ' ' != args.front() )
    args.insert ( args.begin(), ' ' );

  // Make it a valid c-style string.
  args.push_back ( '\0' );

#ifdef _MSC_VER

  // Initialize startup and process info.
  STARTUPINFO si;
  ::ZeroMemory ( &si, sizeof ( STARTUPINFO ) );
  Helper::Data *data ( new Helper::Data );

  // Flags for the process.
  const unsigned int flags ( CREATE_NO_WINDOW );

  // Start the process. This immediately returns and does not 
  // wait for the new process to finish initialization.
  Usul::System::LastError::init();
  const BOOL result ( ::CreateProcessA ( executable.c_str(), &args[0], 
                                        0x0, 0x0, FALSE, flags, 0x0, 0x0,
                                        &si, &(data->processInfo()) ) );
  if ( FALSE == result )
    Helper::throwException ( "Error 1816237620: Failed to create process", executable, arguments, 0 );

  // Keep track of the process information.
  _data = data;

#else

  Usul::System::LastError::init();

  typedef Usul::System::Detail::ProcessImpl InternalProcess;
  typedef std::auto_ptr < InternalProcess > InternalProcessPtr;

  // Make new process and start it.
  InternalProcessPtr impl ( new InternalProcess() );
  const bool result ( impl->start ( executable, arguments ) );

  // See if it started.
  if ( false == result )
  {
    Helper::throwException ( "Error 1816237620: Failed to create process", executable, arguments, 0 );
  } 

  // Assign our internal pointer and release the local smart-pointer.
  _data = impl.release();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Process::~Process()
{
  Usul::Functions::noThrow ( boost::bind ( &Usul::System::Process::_destroy, this ), "3380342559" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Process::_destroy()
{
  this->stop();
  Helper::deleteProcessInfo ( _data );
  _data = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the system's process id.
//
///////////////////////////////////////////////////////////////////////////////

Process::ID Process::id() const
{
  Guard guard ( this );
#ifdef _MSC_VER
  return ( Helper::getProcessInfo ( _data ).dwProcessId );
#else
  Detail::ProcessImpl* impl ( reinterpret_cast< Detail::ProcessImpl*> ( _data ) );
  if ( 0x0 != impl )
    return impl->id();
  
  return -1;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the process running?
//
///////////////////////////////////////////////////////////////////////////////

bool Process::isRunning() const
{
  Guard guard ( this );

#ifdef _MSC_VER
  
  if ( true == _hasBeenStopped )
    return false;

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );
  DWORD exitCode ( 0 );

  Usul::System::LastError::init();

  if ( FALSE == ::GetExitCodeProcess ( pi.hProcess, &exitCode ) )
    Helper::throwException ( "Error 3041975851: Unable to determine state of process", _executable, _arguments, pi.dwProcessId );

  // If the exit code is something other than "still active" it's not running.
  if ( STILL_ACTIVE != exitCode )
    return false;

  Usul::System::LastError::init();

  // If we get here then we don't know if it is really still active, or if it 
  // just happened to return with a numerical exit code equal to STILL_ACTIVE.

  // This will return immediately because of the zero timeout, and will tell 
  // us if the process is still running.
  const DWORD result ( ::WaitForSingleObject ( pi.hProcess, 0 ) );
  switch ( result )
  {
    case WAIT_OBJECT_0:
      return false;
    case WAIT_TIMEOUT:
      return true;
    default:
      Helper::throwException ( "Error 1676419807: Unable to determine state of process", _executable, _arguments, pi.dwProcessId );
      return false; // Never gets here.
  }

#else
  Detail::ProcessImpl* impl ( reinterpret_cast< Detail::ProcessImpl*> ( _data ) );
  if ( 0x0 != impl )
    return impl->isRunning();
  return false;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the process.
//
///////////////////////////////////////////////////////////////////////////////

void Process::stop()
{
  Guard guard ( this );

  if ( false == this->isRunning() )
    return;

#ifdef _MSC_VER
  
  // Set this now.
  _hasBeenStopped = true;

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );

  // Should not happen.
  if ( 0x0 == pi.hProcess )
    return;

  // This is the hammer.
  ::TerminateProcess ( pi.hProcess, 1 );

  // Done with these handles.
  Helper::closeHandle ( pi.hProcess );
  Helper::closeHandle ( pi.hThread );

  // Note: do not clean up standard output handles here because the data 
  // in the pipe could still be valid even though we just terminated the 
  // process that made it. Just a hunch...

#else
  Detail::ProcessImpl* impl ( reinterpret_cast< Detail::ProcessImpl*> ( _data ) );
  if ( 0x0 != impl )
    _hasBeenStopped = impl->stop();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for the process.
//
///////////////////////////////////////////////////////////////////////////////

void Process::wait ( unsigned long milliseconds )
{
  Guard guard ( this );

  if ( true == _hasBeenStopped )
    return;

#ifdef _MSC_VER

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );
  ::WaitForSingleObject ( pi.hProcess, milliseconds );

#else

  Detail::ProcessImpl* impl ( reinterpret_cast< Detail::ProcessImpl*> ( _data ) );
  if ( 0x0 != impl )
    impl->wait ( milliseconds );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current process's id.
//
///////////////////////////////////////////////////////////////////////////////

Process::ID Process::currentProcessId()
{
#ifdef _MSC_VER
  return GetCurrentProcessId();
#else
  return Detail::ProcessImpl::currentProcessId();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of process running with the given name.
//  http://msdn.microsoft.com/en-us/library/ms686701(VS.85).aspx
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Process::count ( const std::string &name )
{
  // Initialize the count.
  unsigned int count ( 0 );

#ifdef _MSC_VER

  // Take a snapshot of all processes in the system.
  HANDLE snapshot ( ::CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) );
  if ( INVALID_HANDLE_VALUE == snapshot )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 4003403320: Failed to create snapshot of processes. ",
      ( ( true == Usul::System::LastError::has() ) ? Usul::System::LastError::message() : std::string() ) ) );
  }

  // Get info for the first process.
  PROCESSENTRY32 process;
  ::memset ( &process, 0, sizeof ( PROCESSENTRY32 ) );
  process.dwSize = sizeof ( PROCESSENTRY32 );
  if ( FALSE == ::Process32First ( snapshot, &process ) )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 1158464803: Failed to get information about the first process. ",
      ( ( true == Usul::System::LastError::has() ) ? Usul::System::LastError::message() : std::string() ) ) );
  }

  // We check all lower case.
  const std::string lower ( boost::filesystem::basename ( boost::algorithm::to_lower_copy ( name ) ) );

  // Loop over all the processes.
  do
  {
    const std::string current ( boost::filesystem::basename ( boost::algorithm::to_lower_copy ( std::string ( process.szExeFile ) ) ) );
    if ( lower == current )
    {
      ++count;
    }
  }
  while ( TRUE == ::Process32Next ( snapshot, &process ) );

#endif

  // Return the count.
  return count;
}
