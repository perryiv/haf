
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

#include "SceneGraph/Plugins/IO/HSG/SceneGraphHSG.h"
#include "SceneGraph/Plugins/IO/HSG/Parser.h"

#include "Usul/Plugins/Helper.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/filesystem.hpp"

#include <fstream>
#include <stdexcept>

using namespace SceneGraph::IO::HSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneGraphHSG, BaseClass );
USUL_PLUGIN_INITIALIZE ( SceneGraphHSG );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphHSG::SceneGraphHSG() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphHSG::~SceneGraphHSG()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneGraphHSG::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IDocumentFilters::IID:
    return static_cast < Usul::Interfaces::IDocumentFilters * > ( this );
  case SceneGraph::Interfaces::IReadNode::IID:
    return static_cast < SceneGraph::Interfaces::IReadNode * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

std::string SceneGraphHSG::pluginDescription() const
{
  return "SceneGraph plugin for native scene-graph files";
}


/////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
/////////////////////////////////////////////////////////////////////////////

SceneGraphHSG::NodePtr SceneGraphHSG::readSceneGraphNode 
  ( const std::string &file, IUnknown::RefPtr caller )
{
  Parser parser;
  return parser.read ( file, caller );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
/////////////////////////////////////////////////////////////////////////////

SceneGraphHSG::NodePtr SceneGraphHSG::readSceneGraphNode 
  ( std::istream &in, Usul::Types::UInt64 streamSize, IUnknown::RefPtr caller )
{
  Parser parser;
  return parser.read ( in, streamSize, caller );
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if we can read the file.
//
/////////////////////////////////////////////////////////////////////////////

bool SceneGraphHSG::canReadSceneGraphNode ( const std::string &file ) const
{
  const std::string ext ( boost::algorithm::to_lower_copy 
    ( boost::filesystem::extension ( file ) ) );

  return ( ".hsg" == ext );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

void SceneGraphHSG::appendDocumentFilters ( const std::string &s, Filters &filters ) const
{
  const std::string type ( boost::algorithm::to_lower_copy ( s ) );
  if ( ( "open" == type ) || ( "read" == type ) || ( "load" == type ) )
  {
    filters.push_back ( Filters::value_type ( "HAF Scene Graph (*.hsg)", "*.hsg" ) );
  }
}
