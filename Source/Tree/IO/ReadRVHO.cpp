
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Ridiculously Verbose Hierarchical Objects (RVHO) reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/IO/ReadRVHO.h"

#include "Usul/Exceptions/Exceptions.h"

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include <fstream>
#include <map>
#include <stack>

using namespace Tree::IO;


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef Tree::Node Node;
  typedef Node::Attributes Attributes;
  typedef Node::RefPtr NodePtr;
  typedef std::stack < NodePtr > NodeStack;
  typedef boost::function0 < void > Handler;
  typedef std::map < std::string, Handler > Handlers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the attribute.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseAttribute ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Get the top node.
    NodePtr node ( ( true == nodes.empty() ) ? NodePtr() : nodes.top() );
    if ( false == node.valid() )
      return;

    // Initialize the attribute name and value.
    std::string name, value;

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the attribute name?
      if ( "attribute_name_end" == word )
      {
        break;
      }

      // Append the word to the name.
      name += ( ( true == name.empty() ) ? word : ( " " + word ) );
    }

    // The next word should be this.
    {
      std::string word;
      in >> word;
      if ( "attribute_value_begin" != word )
      {
        return;
      }
    }

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the attribute value?
      if ( "attribute_value_end" == word )
      {
        break;
      }

      // Append the word to the value.
      value += ( ( true == value.empty() ) ? word : ( " " + word ) );
    }

    // If we have a name...
    if ( false == name.empty() )
    {
      // Set the attribute.
      node->attribute ( name, value );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the attributes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseAttributes ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the attributes?
      if ( "attributes_end" == word )
      {
        break;
      }

      // This is the only other word we should see.
      else if ( "attribute_name_begin" == word )
      {
        // Look up the handler.
        Handler handler ( handlers[word] );
        if ( handler )
        {
          handler();
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the name.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseName ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Get the top node.
    NodePtr node ( ( true == nodes.empty() ) ? NodePtr() : nodes.top() );
    if ( false == node.valid() )
      return;

    // Initialize the name.
    std::string name;

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the name?
      if ( "name_end" == word )
      {
        node->name ( name );
        break;
      }

      // Append the word to the name.
      name += ( ( true == name.empty() ) ? word : ( " " + word ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseValue ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Get the top node.
    NodePtr node ( ( true == nodes.empty() ) ? NodePtr() : nodes.top() );
    if ( false == node.valid() )
      return;

    // Initialize the value.
    std::string value;

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the name?
      if ( "value_end" == word )
      {
        node->value ( value );
        break;
      }

      // Append the word to the value.
      value += ( ( true == value.empty() ) ? word : ( " " + word ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the children.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseChildren ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the children?
      if ( "children_end" == word )
      {
        break;
      }

      // This is the only other word we should see.
      else if ( "node_begin" == word )
      {
        // Look up the handler.
        Handler handler ( handlers[word] );
        if ( handler )
        {
          handler();
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the node.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseNode ( std::istream &in, NodeStack &nodes, Handlers &handlers )
  {
    // Set new top node is appropriate.
    {
      // Get the parent node, which may be null.
      NodePtr parent ( ( true == nodes.empty() ) ? NodePtr() : nodes.top() );

      // Make a new node and push it onto the stack.
      NodePtr node ( new Node ( "" ) );
      nodes.push ( node );

      // Add this node to the parent if there is one.
      if ( true == parent.valid() )
      {
        parent->append ( node );
      }
    }

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word.
      std::string word;
      in >> word;

      // Is this the end of the node?
      if ( "node_end" == word )
      {
        break;
      }

      // Look up the handler.
      Handler handler ( handlers[word] );
      if ( handler )
      {
        handler();
      }
    }

    // We're done with this node so pop it.
    if ( false == nodes.empty() )
    {
      nodes.pop();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to parse the stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void parseStream ( std::istream &in, NodeStack &nodes )
  {
    // Register handlers for keywords.
    Handlers handlers;
    handlers["node_begin"] = boost::bind ( 
      &Helper::parseNode, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );
    handlers["name_begin"] = boost::bind ( 
      &Helper::parseName, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );
    handlers["value_begin"] = boost::bind ( 
      &Helper::parseValue, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );
    handlers["attributes_begin"] = boost::bind ( 
      &Helper::parseAttributes, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );
    handlers["attribute_name_begin"] = boost::bind ( 
      &Helper::parseAttribute, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );
    handlers["children_begin"] = boost::bind ( 
      &Helper::parseChildren, 
      boost::ref ( in ), 
      boost::ref ( nodes ), 
      boost::ref ( handlers ) );

    // Loop until the end of the file.
    while ( EOF != in.peek() )
    {
      // Read the word and look up the handler.
      std::string word;
      in >> word;
      Handler handler ( handlers[word] );
      if ( handler )
      {
        handler();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the rvho file and build the tree.
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr ReadRVHO::read ( const std::string &file )
{
  // Handle bad input.
  if ( true == file.empty() )
  {
    throw Usul::Exceptions::Error 
      ( "7681691400", "Empty file name given" );
  }

  // Open file.
  std::ifstream in ( file.c_str() );

  // Handle bad stream.
  if ( false == in.is_open() )
  {
    throw Usul::Exceptions::Error 
      ( "2876437926", "failed to open file for reading: " + file );
  }

  // Call other one.
  return ReadRVHO::read ( in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the rvho stream and build the tree.
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr ReadRVHO::read ( std::istream &in )
{
  // The stack of nodes.
  Helper::NodeStack nodes;

  // Make a root node so that there is always one on the stack.
  nodes.push ( NodePtr ( new Node ( "root" ) ) );

  // Parse the stream.
  Helper::parseStream ( in, nodes );

  // Handle no nodes.
  if ( true == nodes.empty() )
  {
    return NodePtr();
  }

  // Get the top node.
  NodePtr root ( nodes.top() );

  // Handle no children (which means nothing was parsed).
  Node::Children children ( root->children() );
  if ( true == children.empty() )
  {
    return NodePtr();
  }

  // If there is only one child then replace our root with that child.
  if ( 1 == children.size() )
  {
    root = children.front();
  }

  // Handle no name.
  if ( true == root->name().empty() )
  {
    return NodePtr();
  }

  // Return the root node.
  return root;
}
