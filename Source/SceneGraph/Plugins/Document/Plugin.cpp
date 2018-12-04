
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Plugins/Document/Plugin.h"
#include "SceneGraph/Plugins/Document/Document.h"
#include "SceneGraph/Interfaces/IReadNode.h"

#include "Usul/Plugins/Helper.h"
#include "Usul/Plugins/Manager.h"

using namespace SceneGraph::Plugins;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Plugin, BaseClass );
USUL_PLUGIN_INITIALIZE ( Plugin );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Plugin::Plugin() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Plugin::~Plugin()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Plugin::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IDocumentCreate::IID:
    return static_cast < Usul::Interfaces::IDocumentCreate * > ( this );
  case Usul::Interfaces::IDocumentFilters::IID:
    return static_cast < Usul::Interfaces::IDocumentFilters * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Can we create the file?
//
/////////////////////////////////////////////////////////////////////////////

bool Plugin::canCreateDocument ( const std::string &file ) const
{
  // Ask for all appropriate plugins.
  typedef SceneGraph::Interfaces::IReadNode IReadNode;
  typedef Usul::Plugins::Manager::UnknownSet UnknownSet;
  UnknownSet unknowns;
  Usul::Plugins::Manager::instance().findAll ( IReadNode::IID, unknowns );

  // Loop through the plugins.
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    IReadNode::QueryPtr reader ( *i );
    if ( true == reader.valid() )
    {
      if ( true == reader->canReadSceneGraphNode ( file ) )
      {
        return true;
      }
    }
  }

  // If we get to here then we cannot read the file.
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create the document.
//
/////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Plugin::createDocument ( IUnknown::RefPtr caller )
{
  return IUnknown::QueryPtr ( new SceneGraph::Plugins::Document );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

std::string Plugin::pluginDescription() const
{
  return "Scene graph document plugin";
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

void Plugin::appendDocumentFilters ( const std::string &s, Filters &filters ) const
{
  // Ask for all reader plugins.
  typedef SceneGraph::Interfaces::IReadNode IReadNode;
  typedef Usul::Interfaces::IDocumentFilters IDocumentFilters;
  typedef Usul::Plugins::Manager::UnknownSet UnknownSet;
  UnknownSet unknowns;
  Usul::Plugins::Manager::instance().findAll ( IReadNode::IID, unknowns );

  // Loop through the plugins.
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    // Ask each reader plugin if it can supply document filters.
    IDocumentFilters::QueryPtr df ( *i );
    if ( true == df.valid() )
    {
      // Append the filters.
      df->appendDocumentFilters ( s, filters );
    }
  }
}
