
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Ridiculously Verbose Hierarchical Objects (RVHO) writer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/IO/WriteRVHO.h"

#include "Usul/Exceptions/Exceptions.h"

#include <fstream>

using namespace Tree::IO;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to traverse the tree.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const Tree::Node::RefPtr node, 
                  const std::string &indent, 
                  std::ostream &out )
  {
    typedef Tree::Node Node;

    // Handle invalid node.
    if ( false == node.valid() )
      return;

    // Write start of node.
    {
      // Handle no name.
      const std::string name ( node->name() );
      if ( true == name.empty() )
        return;

      // Write start of node.
      out << indent << "node_begin" << '\n';
      out << indent << "  name_begin" << '\n';
      out << indent << "    " << name << '\n';
      out << indent << "  name_end" << '\n';
    }

    // Write value if there is one.
    {
      const std::string value ( node->value() );
      if ( false == value.empty() )
      {
        out << indent << "  value_begin" << '\n';
        out << indent << "    " << value << '\n';
        out << indent << "  value_end" << '\n';
      }
    }

    // Write attributes.
    {
      const Node::Attributes attr ( node->attributes() );
      if ( false == attr.empty() )
      {
        out << indent << "  attributes_begin" << '\n';
        for ( Node::Attributes::const_iterator a = attr.begin(); a != attr.end(); ++a )
        {
          const std::string &name ( a->first );
          const std::string &value ( a->second );
          if ( false == name.empty() )
          {
            out << indent << "    attribute_name_begin" << '\n';
            out << indent << "      " << name << '\n';
            out << indent << "    attribute_name_end" << '\n';
            out << indent << "    attribute_value_begin" << '\n';
            out << indent << "      " << value << '\n';
            out << indent << "    attribute_value_end" << '\n';
          }
        }
        out << indent << "  attributes_end" << '\n';
      }
    }

    // Write children.
    {
      const Node::Children children ( node->children() );
      if ( false == children.empty() )
      {
        out << indent << "  children_begin" << '\n';
        for ( Node::Children::const_iterator c = children.begin(); c != children.end(); ++c )
        {
          const Node::RefPtr &child ( *c );
          if ( true == child.valid() )
          {
            Helper::traverse ( child, indent + "    ", out );
          }
        }
        out << indent << "  children_end" << '\n';
      }
    }

    // Write end of node.
    {
      out << indent << "node_end" << '\n';
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write node to file using rvho format.
//
///////////////////////////////////////////////////////////////////////////////

void WriteRVHO::write ( const Tree::Node::RefPtr node, const std::string &file )
{
  // Handle bad input.
  if ( false == node.valid() )
    return;

  // Open file.
  std::ofstream out ( file.c_str() );
  if ( false == out.is_open() )
  {
    throw Usul::Exceptions::Error 
      ( "2353921307", "failed to open file for writing: " + file );
  }

  // Call other one.
  WriteRVHO::write ( node, out );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write node to the stream using rvho format.
//
///////////////////////////////////////////////////////////////////////////////

void WriteRVHO::write ( const Tree::Node::RefPtr node, std::ostream &out )
{
  // Handle bad input.
  if ( false == node.valid() )
    return;

  // Traverse the tree.
  std::string indent;
  Helper::traverse ( node, indent, out );
}
