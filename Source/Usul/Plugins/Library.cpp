
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a dynamically loaded library.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Plugins/Library.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include "boost/bind.hpp"
#include "boost/filesystem.hpp"

#ifdef _WIN32
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <dlfcn.h>
#endif

using namespace Usul::Plugins;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::Library ( const std::string &file ) : BaseClass(),
  _module ( 0x0 ),
  _file ( file )
{
  // Initialize the last error.
  System::LastError::init();

  // Must have valid file name.
  if ( true == _file.empty() )
  {
    throw std::invalid_argument ( "Error 2662409693: Empty file name given when loading library" );
  }

  // File must exist.
  if ( false == boost::filesystem::exists ( _file ) )
  {
    throw std::invalid_argument ( Usul::Strings::format
      ( "Error 3024069386: Library file '", _file, ", does not exist" ) );
  }

#ifdef _MSC_VER
  _module = reinterpret_cast < ModuleHandle > ( ::LoadLibrary ( _file.c_str() ) );
#else
  _module = reinterpret_cast < ModuleHandle > ( ::dlopen ( _file.c_str(), RTLD_LAZY | RTLD_GLOBAL ) );
#endif

  // See if it worked.
  if ( 0x0 == _module )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 3942529130: failed to load library '", _file, "', ",
      "System error number: ", System::LastError::number(), ", ",
      "System error message: ", System::LastError::message() ) );
  }

  // Get the debug function. Note: at one time g++ did not allow a reinterpret_cast.
  typedef bool (*DebugFunction)();
  DebugFunction isDebugBuild ( Usul::Cast::unsafe<DebugFunction> ( this->function ( "usul_is_debug_build" ) ) );

  // If we found it then use it.
  if ( 0x0 != isDebugBuild )
  {
  #ifdef _DEBUG
    if ( false == isDebugBuild() )
  #else
    if ( true == isDebugBuild() )
  #endif
    {
      throw std::runtime_error ( Usul::Strings::format
        ( "Error 4210150186: Mismatched build mode in library: ", _file ) );
    }
  }

  // Look for the function used to initialize a plugin.
  typedef void (*Initialize)();
  Initialize initialize ( Usul::Cast::unsafe<Initialize> ( this->function ( "usul_plugin_initialize" ) ) );

  // If we found it then call it.
  if ( 0x0 != initialize )
  {
    initialize();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::~Library()
{
  Usul::Functions::noThrow ( boost::bind ( &Library::free, this ), "2657646620" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Free the library.
//
///////////////////////////////////////////////////////////////////////////////

void Library::free()
{
  typedef Usul::Atomic::Integer < ModuleHandle > Module;
  typedef void (*Finalize)();

  // Look for the function to finalize a plugin before we delete _module.
  Finalize finalize ( Usul::Cast::unsafe<Finalize> ( this->function ( "usul_plugin_finalize" ) ) );

  // If we found the finalize function then call it.
  if ( 0x0 != finalize )
  {
    finalize();
  }

  // Copy module and store new value.
  ModuleHandle module ( _module.fetch_and_store ( 0x0 ) );

  // Handle null modules.
  if ( 0x0 == module )
    return;

  // Initialize the last error.
  System::LastError::init();

#ifdef _MSC_VER
  bool result ( 0 != ::FreeLibrary ( reinterpret_cast < HMODULE > ( module ) ) );
#else
  bool result ( 0 == ::dlclose ( reinterpret_cast < void * > ( module ) ) );
#endif

  // Make sure it worked.
  if ( false == result )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 4286324342: failed to free library '", _file, ", ",
      "System error number: ", System::LastError::number(), ", ",
      "System error message: ", System::LastError::message() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the address of the function.
//
///////////////////////////////////////////////////////////////////////////////

Library::Function Library::function ( const std::string &name ) const
{
  // Handle bad string.
  if ( name.empty() )
    return 0x0;

  // Copy module pointer.
  ModuleHandle module ( _module );

  // Handle bad module.
  if ( 0x0 == module )
    return 0x0;

#ifdef _WIN32
  return reinterpret_cast < Function > ( ::GetProcAddress ( reinterpret_cast < HMODULE > ( module ), name.c_str() ) );
#else
  return reinterpret_cast < Function > ( ::dlsym ( reinterpret_cast < void * > ( module ), name.c_str() ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Library::file() const
{
  return _file;
}
