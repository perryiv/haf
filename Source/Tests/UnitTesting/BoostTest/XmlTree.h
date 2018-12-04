
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test functions for XmlTree library.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Expat/Reader.h"
#include "XmlTree/Loader.h"
#include "XmlTree/Node.h"
#include "XmlTree/Writer.h"
#include "XmlTree/XercesLife.h"

#include "Usul/File/Temp.h"
#include "Usul/Scope/RemoveFile.h"

#include "boost/test/unit_test.hpp"

#include <fstream>


namespace Tests {
namespace XmlTree {


///////////////////////////////////////////////////////////////////////////////
//
//  To shorten lines below.
//
///////////////////////////////////////////////////////////////////////////////

using namespace ::XmlTree;
using namespace ::XmlTree::Expat;


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test001()
{
  Node::RefPtr root ( new Node ( "root" ) );
  root->append ( "child", "1" );
  root->append ( "child", 2 );
  BOOST_CHECK_EQUAL ( 2, root->numChildren() );
  BOOST_CHECK_EQUAL ( "root", root->name() );
  BOOST_CHECK_EQUAL ( true, root->value().empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test002()
{
  XercesLife scopedXercesLife;

  const std::string file ( ::Usul::File::Temp::file() );
  ::Usul::Scope::RemoveFile remove ( file );

  std::ostringstream contents;
  contents << "<root>\n";
  contents << "  <child>1</child>\n";
  contents << "  <child>2.3</child>\n";
  contents << "  <child attr1=\"my attr 1\" attr2=\"my attr 2\">\n";
  contents << "    <sub>hi</sub>\n";
  contents << "  </child>\n";
  contents << "  <child attr1=\"my attr 1\" attr2=\"my attr 2\">8.1</child>\n";
  contents << "</root>\n";

  {
    std::ofstream out ( file.c_str() );
    out << contents.str();
  }

  {
    Node::RefPtr node ( Loader::load ( file ) );
    BOOST_CHECK_EQUAL ( true, node.valid() );
    BOOST_CHECK_EQUAL ( 4, node->numChildren() );
    Node::Children c ( node->children() );
    BOOST_CHECK_EQUAL ( 4, c.size() );
    node = c[1];
    BOOST_CHECK_EQUAL ( true, node.valid() );
    BOOST_CHECK_EQUAL ( "2.3", node->value() );
  }

  {
    Node::RefPtr node ( Loader::load ( file ) );
    std::ostringstream out;
    Writer::write ( node, out, false );
    BOOST_CHECK_EQUAL ( out.str(), contents.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test003()
{
  const std::string file ( ::Usul::File::Temp::file() );
  ::Usul::Scope::RemoveFile remove ( file );

  std::ostringstream contents;
  contents << "<root>\n";
  contents << "  <a>1</a>\n";
  contents << "  <b>2.3</b>\n";
  contents << "  <c attr1=\"my attr 1\" attr2=\"my attr 2\">\n";
  contents << "    <sub>hi</sub>\n";
  contents << "  </c>\n";
  contents << "  <d>\n";
  contents << "    <sub attr1=\"my attr abc\">hi</sub>\n";
  contents << "  </d>\n";
  contents << "  <e attr1=\"my attr 3\"><sub>hi</sub>\n";
  contents << "  </e>\n";
  contents << "  <f attr1=\"my attr 4\"><sub>hi</sub>\n</f>\n";
  contents << "  <g attr1=\"my attr 5\" attr2=\"my attr 6\">8.1</g>\n";
  contents << "  <h attr1=\"my attr 7\" attr2=\"my attr 8\"/>\n";
  contents << "  <i attr1=\"my attr 9\" attr2=\"my attr 10\" />\n";
  contents << "  <j attr1=\"my attr 11\"\n";
  contents << "     attr2=\"my attr 12\" />\n";
  contents << "  <k \n";
  contents << "     attr1=\"my attr 13\"\n";
  contents << "     attr2=\"my attr 14\" />\n";
  contents << "  <l \n";
  contents << "     attr1=\"my attr 15\"\n";
  contents << "     attr2=\"my attr 16\"\n";
  contents << "  />\n";
  contents << "  <m \n";
  contents << "     attr1=\"my attr 17\"\n";
  contents << "     attr2=\"my attr 18\">\n";
  contents << "  </m>\n";
  contents << "</root>\n";

  {
    std::ofstream out ( file.c_str() );
    out << contents.str();
  }

  {
    Reader reader;
    Node::RefPtr node1 ( reader.read ( file ) );
    Node::RefPtr node2 ( Loader::load ( file ) );
    std::ostringstream out1;
    std::ostringstream out2;
    Writer::write ( node1, out1, false );
    Writer::write ( node2, out2, false );
    BOOST_CHECK_EQUAL ( out1.str(), out2.str() );
  }
}


} // namespace XmlTree
} // namespace Tests
