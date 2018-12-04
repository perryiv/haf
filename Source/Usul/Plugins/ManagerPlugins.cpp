
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach and Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Manager of plugins.
//
//  Note: this file is not named "Manager.cpp" because VC++ 9 could not 
//  distinguish it from the Usul/Documents/Manager.cpp.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>

using namespace Usul::Plugins;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the singleton at startup to avoid problems with construction 
//  in multi-threaded environments.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      #ifdef USUL_AUTO_CREATE_SINGLETON_PLUGIN_MANAGER
      Usul::Plugins::Manager::instance();
      #endif
    }
    ~Init()
    {
      #ifdef USUL_AUTO_DELETE_SINGLETON_PLUGIN_MANAGER
      Usul::Plugins::Manager::destroy();
      #endif
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Plugins
  {
    namespace Detail
    {
      Usul::Plugins::Manager *_instance ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager& Manager::instance()
{
  if ( 0x0 == Usul::Plugins::Detail::_instance )
  {
    Usul::Plugins::Detail::_instance = new Manager();
  }
  return *Usul::Plugins::Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
  Manager *m ( Usul::Plugins::Detail::_instance );
  Usul::Plugins::Detail::_instance = 0x0;
  delete m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() : 
  _plugins(),
  _libraries()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  Usul::Functions::noThrow ( boost::bind ( &Manager::_destroy, this ), "3658807590" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_destroy()
{
  this->removeAll ( &std::cout );
  this->unloadAll ( &std::cout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::add ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  if ( true == unknown.valid() )
  {
    Plugins::Guard guard ( _plugins.mutex() );
    _plugins.getReference().push_back ( unknown );
  }

  Usul::Interfaces::IPlugin::QueryPtr plugin ( unknown );
  if ( true == plugin.valid() )
  {
    plugin->pluginInitialize();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the plugins
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeAll ( std::ostream *out )
{
  // Copy the set and clear our member.
  UnknownSet plugins;
  {
    Plugins::Guard guard ( _plugins.mutex() );
    plugins = _plugins;
    _plugins.clear();
  }

  // Loop through the copy.
  while ( false == plugins.empty() )
  {
    // Get the plugin.
    UnknownSet::iterator i ( plugins.begin() );
    IUnknown::RefPtr plugin ( *i );

    // Finalize it.
    this->_finalize ( plugin, out );

    // Remove it from the local copy.
    plugins.erase ( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize the plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_finalize ( Usul::Interfaces::IUnknown::RefPtr unknown, std::ostream *out ) 
{
  Usul::Interfaces::IPlugin::QueryPtr plugin ( unknown );
  if ( true == plugin.valid() )
  {
    // Feedback if the caller wants it.
    if ( 0x0 != out )
    {
      (*out) << Usul::Strings::format ( "Releasing plugin: ", plugin->pluginName(), '\n' ) << std::flush;
    }

    // Tell the plugin it's about to be released.
    Usul::Functions::noThrow ( boost::bind ( &Usul::Interfaces::IPlugin::pluginFinalize, plugin.get() ), "2550101349" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( const std::string &file )
{
  Library::RefPtr library ( new Library ( file ) );
  Libraries::Guard guard ( _libraries.mutex() );
  _libraries.getReference().push_back ( library );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unload all the libraries.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::unloadAll ( std::ostream *out )
{
  // Copy the set and clear our member.
  LibrarySet libraries;
  {
    Libraries::Guard guard ( _libraries.mutex() );
    libraries = _libraries;
    _libraries.clear();
  }

  // Loop through the copy and remove the libraries.
  while ( false == libraries.empty() )
  {
    libraries.erase ( libraries.begin() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate used for finding plugins.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct ImplementsInterface
  {
    ImplementsInterface ( unsigned long iid ) : _iid ( iid ){}
    template < class T > bool operator () ( T v ) const
    {
      return ( ( true == v.valid() ) ? ( 0x0 != v->queryInterface ( _iid ) ) : false );
    }
  private:
    const unsigned long _iid;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first plugin that matches the interface.
//
///////////////////////////////////////////////////////////////////////////////

Manager::UnknownPtr Manager::findFirst ( unsigned long iid ) const
{
  return this->findFirstIf ( Helper::ImplementsInterface ( iid ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the plugins that match the predicate.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::findAll ( unsigned long iid, UnknownSet &answer ) const
{
  this->findAllIf ( Helper::ImplementsInterface ( iid ), answer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of plugin names.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Strings Manager::names ( bool sort ) const
{
  Manager::Strings names;
  UnknownSet unknowns ( _plugins );
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    Usul::Interfaces::IPlugin::QueryPtr plugin ( *i );
    if ( true == plugin.valid() )
    {
      names.push_back ( plugin->pluginName() );
    }
  }

  if ( true == sort )
  {
    std::sort ( names.begin(), names.end() );
  }

  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print message about loaded plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::print ( std::ostream &out ) const
{
  std::ostringstream stream;

  const Manager::Strings names ( this->names() );
  stream << names.size() << ( ( 1 == names.size() ) ? ( " plugin" ) : ( " plugins" ) );
  if ( false == names.empty() )
  {
    stream << ": ";
    std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( stream, "; " ) );
  }
  stream << '\n';
  out << stream.str() << std::flush;
}
