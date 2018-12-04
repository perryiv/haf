
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
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Application.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include "windows.h"
# include "excpt.h"
#endif

using namespace Helios;


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helios
{
  namespace Detail
  {
    Helios::Application *_instance ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the single instance.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init ( int &argc, char **argv )
{
  if ( 0x0 != Helios::Detail::_instance )
  {
    throw std::runtime_error ( "Error 7232488000: Application already initialized" );
  }
  Helios::Detail::_instance = new Application ( argc, argv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Application& Application::instance()
{
  if ( 0x0 == Helios::Detail::_instance )
  {
    throw std::runtime_error ( "Error 2974416871: Must call Application::init() first" );
  }
  return *Helios::Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

void Application::destroy()
{
  Application *app ( Helios::Detail::_instance );
  Helios::Detail::_instance = 0x0;
  delete app;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( int &argc, char **argv ) : BaseClass ( argc, argv ),
  _threadCheck(),
  _mainWindow ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  _mainWindow = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to handle Windows structured exception.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  int handleStructuredException ( DWORD code )
  {
    const std::string message ( Usul::System::LastError::message ( code ) );
    std::cout << Usul::Strings::format ( 
      "Error 2917508413: structured exception ", code, " generated when processing event.", 
      ( ( false == message.empty() ) ? message : "" ) ) << std::endl;
    return EXCEPTION_EXECUTE_HANDLER;
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of every event.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::notify ( QObject *object, QEvent *event )
{
  #ifdef _MSC_VER

    __try 
    {
      return this->_notify ( object, event );
    }

    __except ( Helper::handleStructuredException ( ::GetExceptionCode() ) )
    {
      return false;
    }

  #else

    return this->_notify ( object, event );

  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of every event.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_notify ( QObject *object, QEvent *event )
{
  try
  {
    return BaseClass::notify ( object, event );
  }
  catch ( const Usul::Exceptions::Cancelled & )
  {
    #ifdef _DEBUG
    std::cout << "Message 3196212174: Operation cancelled by user\n" << std::flush;
    #endif
  }
  catch ( const std::exception &e )
  {
    std::cout << Usul::Strings::format ( "Error 8033290760: ", e.what(), '\n' ) << std::flush;
  }
  catch ( ... )
  {
    std::cout << Usul::Strings::format ( "Error 9522173940: exception generated when processing event\n" ) << std::flush;
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the main-window.
//
///////////////////////////////////////////////////////////////////////////////

MainWindows::MainWindow *Application::mainWindow()
{
  return _mainWindow;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the main-window.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setMainWindow ( MainWindows::MainWindow *mw )
{
  _threadCheck.throwIfDifferentThread();
  _mainWindow = mw;
}
