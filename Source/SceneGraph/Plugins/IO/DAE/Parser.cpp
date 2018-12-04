
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The parser class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning ( disable : 4503 ) // Decorated name length exceeded.
#endif

#include "SceneGraph/Plugins/IO/DAE/Parser.h"
#include "SceneGraph/Plugins/IO/DAE/Builders.h"
#include "SceneGraph/Nodes/Groups/Group.h"
#include "SceneGraph/Nodes/Shapes/Shape.h"
#include "SceneGraph/State/Container.h"

#include "Tree/Find.h"
#include "Tree/Interfaces/IReadXML.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"
#include "boost/filesystem.hpp"

#include <fstream>
#include <stdexcept>

using namespace SceneGraph::IO::DAE;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef BaseBuilder::NodeMaps NodeMaps;
typedef BaseBuilder::Builders Builders;
typedef BaseBuilder::NodeData NodeData;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser() : 
  _threadCheck()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::~Parser()
{
  USUL_TRY_BLOCK
  {
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2348290333" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Find the first node with the given attribute.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline NodeData findFirst ( const std::string &name, 
                              const std::string &value, 
                              NodeMaps &nodes )
  {
    typedef Tree::Node::RefPtr Ptr;
    BaseBuilder::NodeSequence &ns ( nodes[name][value] );
    return ( ( ns.empty() ) ? NodeData() : ns.front() );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Traverse the tree and build the maps of nodes.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void buildMaps ( const Tree::Node::RefPtr &node, NodeMaps &nodes )
  {
    // Handle null nodes.
    if ( false == node.valid() )
      return;

    // Get the node's attributes.
    typedef Tree::Node::Attributes Attributes;
    const Attributes a ( node->attributes() );

    // Loop through the attributes.
    for ( Attributes::const_iterator i = a.begin(); i != a.end(); ++i )
    {
      // Get the name and value of the attribute.
      const std::string &name ( i->first );
      const std::string &value ( i->second );

      // Store in the map.
      nodes[name][value].push_back ( node );
    }

    // Get the children.
    typedef Tree::Node::Children Children;
    const Children c ( node->children() );

    // Traverse all the children.
    for ( Children::const_iterator i = c.begin(); i != c.end(); ++i )
    {
      Helper::buildMaps ( *i, nodes );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::Nodes::Node::RefPtr buildGeometry
    ( const Tree::Node::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    typedef SceneGraph::Nodes::Groups::Group Group;
    return SceneGraph::Nodes::Node::RefPtr ( new Group );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the state.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::State::Container::RefPtr buildState
    ( const Tree::Node::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    return SceneGraph::State::Container::RefPtr ( new SceneGraph::State::Container );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the instance geometry.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::Nodes::Node::RefPtr buildInstanceGeometry
    ( const Tree::Node::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    // Handle null nodes.
    if ( false == node.valid() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Should have this name.
    if ( "instance_geometry" != node->name() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Should have a url attribute.
    std::string url ( node->attribute ( "url" ) );
    url.erase ( ( url.empty() ) ? url.end() : url.begin() );
    if ( true == url.empty() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Get the geometry pointed to by the url.
    NodeData geometry ( Helper::findFirst ( "id", url, nodes ) );
    Tree::Node::RefPtr &xg ( geometry.get<0>() );
    if ( false == xg.valid() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Get the geometry. Make it if we have to.
    SceneGraph::Nodes::Node::RefPtr &sg ( geometry.get<1>() );
    if ( false == sg.valid() )
    {
      sg = Helper::buildGeometry ( xg, nodes, builders );
    }

    // The group that holds everything.
    typedef SceneGraph::Nodes::Groups::Group Group;
    Group::RefPtr group ( new Group );

    // The instance-geometry should have at least one child.
    typedef Tree::Node::Children Children;
    const Children c ( node->children() );
    if ( false == c.empty() )
    {
      // Add the state if it's a shape.
      typedef SceneGraph::Nodes::Shapes::Shape Shape;
      Shape::RefPtr shape ( dynamic_cast < Shape * > ( sg.get() ) );
      if ( true == shape.valid() )
      {
        shape->stateContainer ( Helper::buildState ( c.front(), nodes, builders ) );
      }
    }

    // Add the geometry to the group.
    group->append ( sg );

    // Return the group.
    return group;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the scene node.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::Nodes::Node::RefPtr buildSceneNode
    ( const Tree::Node::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    // Handle null nodes.
    if ( false == node.valid() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Should have this name.
    if ( "node" != node->name() )
      return SceneGraph::Nodes::Node::RefPtr();

    // The group that holds everything.
    typedef SceneGraph::Nodes::Groups::Group Group;
    Group::RefPtr group ( new Group );

    // Loop through the children.
    typedef Tree::Node::Children Children;
    const Children c ( node->children() );
    for ( Children::const_iterator i = c.begin(); i != c.end(); ++i )
    {
      // Get the child node.
      Children::const_reference child ( *i );
      if ( true == child.valid() )
      {
        // Look up the builder.
        BaseBuilder::RefPtr builder ( builders[child->name()] );
        if ( true == builder.valid() )
        {
          SceneGraph::Nodes::Node::RefPtr scene 
            ( builder->build ( child, nodes, builders ) );
          if ( true == scene.valid() )
          {
            group->append ( scene );
          }
        }
      }
    }

    // Return the group if it has children.
    return ( ( group->empty() ) ? SceneGraph::Nodes::Node::RefPtr() : group );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::Nodes::Node::RefPtr buildNodeType
    ( const Tree::Node::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    // Handle null nodes.
    if ( false == node.valid() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Look up the builder.
    BaseBuilder::RefPtr builder ( builders[node->name()] );
    if ( false == builder.valid() )
      return SceneGraph::Nodes::Node::RefPtr();

    // Call the builder.
    return builder->build ( node, nodes, builders );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the visual scene.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline SceneGraph::Nodes::Node::RefPtr buildVisualScene 
    ( const Tree::Node::RefPtr &vs, NodeMaps &nodes, Builders &builders )
  {
    // The group that holds everything.
    typedef SceneGraph::Nodes::Groups::Group Group;
    Group::RefPtr group ( new Group );

    // Handle null nodes.
    if ( false == vs.valid() )
      return group;

    // Get the children.
    typedef Tree::Node::Children Children;
    const Children c ( vs->children() );

    // Loop through the children.
    for ( Children::const_iterator i = c.begin(); i != c.end(); ++i )
    {
      SceneGraph::Nodes::Node::RefPtr child 
        ( Helper::buildNodeType ( *i, nodes, builders ) );
      if ( true == child.valid() )
      {
        group->append ( child );
      }
    }

    // Return the top-level group.
    return group;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
/////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::read 
  ( const std::string &name, IUnknown::RefPtr caller )
{
  _threadCheck.throwIfDifferentThread();

  if ( false == boost::filesystem::exists ( name ) )
  {
    throw std::invalid_argument ( Usul::Strings::format 
      ( "Error 9706131050: Given file '", name, "' does not exist" ) );
  }

  const Usul::Types::UInt64 size ( boost::filesystem::file_size ( name ) );

  std::ifstream in ( name.c_str() );
  if ( false == in.is_open() )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 4130195740: Unable to open file '", name, "' for reading" ) );
  }

  return this->read ( in, size, caller );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
/////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::read 
  ( std::istream &in, Usul::Types::UInt64 streamSize, IUnknown::RefPtr caller )
{
  _threadCheck.throwIfDifferentThread();

  // The map of nodes.
  NodeMaps nodes;

  // Load the builders.
  Builders builders;
  builders["node"] = SceneGraph::IO::DAE::makeBuilder 
    ( boost::bind ( &Helper::buildSceneNode, _1, _2, _3 ) );
  builders["instance_geometry"] = DAE::makeBuilder 
    ( boost::bind ( &Helper::buildInstanceGeometry, _1, _2, _3 ) );

  // Find an XML reader.
  typedef Tree::Interfaces::IReadXML IReadXML;
  Usul::Plugins::Manager & pm ( Usul::Plugins::Manager::instance() );
  IReadXML::QueryPtr reader ( pm.findFirst ( IReadXML::IID ) );
  if ( false == reader.valid() )
  {
    throw std::runtime_error 
      ( "Error 1410228054: Unable to find XML reader" );
  }

  // Read the stream.
  Tree::Node::RefPtr root ( reader->readXmlFile ( in, streamSize, caller ) );
  if ( false == root.valid() )
  {
    throw std::runtime_error 
      ( "Error 1887988117: Invalid node returned from XML reader" );
  }

  // Traverse the tree and build the map of nodes.
  Helper::buildMaps ( root, nodes );

  // The default group.
  SceneNode::RefPtr dg ( new SceneGraph::Nodes::Groups::Group );

  // Get the scene node.
  Tree::Node::RefPtr scene ( Tree::Algorithms::findFirst
    ( *root, std::string ( "scene" ), false ) );
  if ( false == scene.valid() )
  {
    return dg;
  }

  // Get the visual scene.
  Tree::Node::RefPtr ivs ( Tree::Algorithms::findFirst
    ( *scene, std::string ( "instance_visual_scene" ), false ) );
  if ( false == ivs.valid() )
  {
    return dg;
  }

  // Should have a url attribute with a leading '#' character.
  std::string url ( ivs->attribute ( "url" ) );
  url.erase ( ( url.empty() ) ? url.end() : url.begin() );
  if ( true == url.empty() )
  {
    return dg;
  }

  // Look for the node with the url's id.
  NodeData vs ( Helper::findFirst ( "id", url, nodes ) );
  if ( false == vs.get<0>().valid() )
  {
    return dg;
  }

  // Process the scene's children.
  return Helper::buildVisualScene ( vs.get<0>(), nodes, builders );
}
