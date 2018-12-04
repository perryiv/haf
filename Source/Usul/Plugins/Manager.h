
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
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PLUGINS_MANAGER_CLASS_H_
#define _USUL_PLUGINS_MANAGER_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Plugins/Library.h"

#include "boost/noncopyable.hpp"

#include <iosfwd>
#include <vector>


namespace Usul {
namespace Plugins {


class USUL_EXPORT Manager : public boost::noncopyable
{
public:

  // Useful typedefs.
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef IUnknown::RefPtr UnknownPtr;
  typedef std::vector < UnknownPtr > UnknownSet;
  typedef Usul::Atomic::Container < UnknownSet > Plugins;
  typedef std::vector < Library::RefPtr > LibrarySet;
  typedef Usul::Atomic::Container < LibrarySet > Libraries;
  typedef std::vector < std::string > Strings;

  // Singleton interface.
  static Manager &                  instance();
  static void                       destroy();

  // Add a plugin.
  void                              add ( IUnknown::RefPtr );

  // Remove all the plugins.
  void                              removeAll ( std::ostream *out = 0x0 );

  // Find the first plugin that implements the interface or matches the predicate.
  UnknownPtr                        findFirst ( unsigned long iid ) const;
  template < class P > UnknownPtr   findFirstIf ( P pred ) const;

  // Find all the plugins that implement the interface.
  void                              findAll ( unsigned long iid, UnknownSet & ) const;
  template < class P > void         findAllIf ( P pred, UnknownSet & ) const;

  // Load the library.
  void                              load ( const std::string &file );

  // Return list of plugin names.
  Strings                           names ( bool sort = true ) const;

  // Print message about loaded plugins.
  void                              print ( std::ostream &out ) const;

  // Unload all the libraries.
  void                              unloadAll ( std::ostream *out = 0x0 );

private:

  Manager();
  ~Manager();

  void                              _destroy();

  void                              _finalize ( IUnknown::RefPtr plugin, std::ostream *out );

  Plugins _plugins;
  Libraries _libraries;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first plugin that matches the predicate.
//
///////////////////////////////////////////////////////////////////////////////

template < class P > inline Manager::UnknownPtr Manager::findFirstIf ( P pred ) const
{
  UnknownSet plugins ( _plugins );
  for ( UnknownSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    UnknownPtr plugin ( *i );
    if ( true == pred ( plugin ) )
    {
      return plugin;
    }
  }
  return UnknownPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the plugins that match the predicate.
//
///////////////////////////////////////////////////////////////////////////////

template < class P > inline void Manager::findAllIf ( P pred, UnknownSet &answer ) const
{
  answer.clear();
  UnknownSet plugins ( _plugins );
  answer.reserve ( plugins.size() );
  for ( UnknownSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    UnknownPtr plugin ( *i );
    if ( true == pred ( plugin ) )
    {
      answer.push_back ( plugin );
    }
  }
}


} // namespace Plugins
} // namespace Usul


#endif // _USUL_PLUGINS_MANAGER_CLASS_H_
