
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML writer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/IO/WriteXML.h"
#include "Tree/Functions.h"

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
  void traverse ( const Tree::Node::RefPtr node, const std::string &indent, 
                  std::ostream &out, bool writeLeadingNewline )
  {
    typedef Tree::Node Node;

    if ( false == node.valid() )
      return;

    // To support writing HTML, check for a name.
    const std::string name ( node->name() );
    if ( false == name.empty() )
    {
      // Write leading newline if we are supposed to.
      if ( true == writeLeadingNewline )
        out << '\n';

      // Write leading tag.
      out << indent << '<' << name;

      // Write attributes.
      const Node::Attributes attr ( node->attributes() );
      for ( Node::Attributes::const_iterator a = attr.begin(); a != attr.end(); ++a )
      {
        if ( false == a->first.empty() && false == a->second.empty() )
        {
          out << ' ' << a->first << "=\"" << Tree::Functions::replaceInvalidXML ( a->second ) << "\"";
        }
      }

      // Finish leading tag.
      out << '>';
    }

    // Write value if there is one.
    const std::string value ( node->value() );
    if ( false == value.empty() )
    {
      out << Tree::Functions::replaceInvalidXML ( value );
    }

    // Write children.
    const Node::Children children ( node->children() );
    for ( Node::Children::const_iterator c = children.begin(); c != children.end(); ++c )
    {
      const Node::RefPtr &n ( *c );
      if ( true == n.valid() )
      {
        Helper::traverse ( n, indent + "  ", out, true );
      }
    }

    // Go to new line if there are children.
    if ( false == children.empty() )
    {
      out << '\n' << indent;
    }

    // Write trailing tag iff there is a name.
    if ( false == name.empty() )
    {
      out << "</" << name << '>';
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write node to file using xml format.
//
///////////////////////////////////////////////////////////////////////////////

void WriteXML::write ( const Tree::Node::RefPtr node, const std::string &file, bool includeHeader )
{
  // Handle bad input.
  if ( false == node.valid() )
    return;

  // Open file.
  std::ofstream out ( file.c_str() );
  if ( false == out.is_open() )
  {
    throw Usul::Exceptions::Error 
      ( "2178129023", "failed to open file for writing: " + file );
  }

  // Call other one.
  WriteXML::write ( node, out, includeHeader );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write node to the stream using xml format.
//
///////////////////////////////////////////////////////////////////////////////

void WriteXML::write ( const Tree::Node::RefPtr node, std::ostream &out, bool includeHeader )
{
  // Handle bad input.
  if ( false == node.valid() )
    return;

  // Write header.
  if ( true == includeHeader )
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

  // Traverse the tree.
  std::string indent;
  Helper::traverse ( node, indent, out, false );

  // Finish with empty line.
  out << '\n';
}
