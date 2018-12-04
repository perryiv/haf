
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

#include "SceneGraph/Plugins/IO/DAE/SceneGraphDAE.h"
#include "SceneGraph/Plugins/IO/DAE/Parser.h"

#include "Usul/Plugins/Helper.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/filesystem.hpp"

#include <fstream>
#include <stdexcept>

using namespace SceneGraph::IO::DAE;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneGraphDAE, BaseClass );
USUL_PLUGIN_INITIALIZE ( SceneGraphDAE );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphDAE::SceneGraphDAE() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphDAE::~SceneGraphDAE()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneGraphDAE::queryInterface ( unsigned long iid )
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

std::string SceneGraphDAE::pluginDescription() const
{
  return "SceneGraph plugin for reading Collada files";
}


/////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
/////////////////////////////////////////////////////////////////////////////

SceneGraphDAE::NodePtr SceneGraphDAE::readSceneGraphNode 
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

SceneGraphDAE::NodePtr SceneGraphDAE::readSceneGraphNode 
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

bool SceneGraphDAE::canReadSceneGraphNode ( const std::string &file ) const
{
  const std::string ext ( boost::algorithm::to_lower_copy 
    ( boost::filesystem::extension ( file ) ) );

  return ( ".dae" == ext );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

void SceneGraphDAE::appendDocumentFilters ( const std::string &s, Filters &filters ) const
{
  const std::string type ( boost::algorithm::to_lower_copy ( s ) );
  if ( ( "open" == type ) || ( "read" == type ) || ( "load" == type ) )
  {
    filters.push_back ( Filters::value_type ( "Collada (*.dae)", "*.dae" ) );
  }
}
