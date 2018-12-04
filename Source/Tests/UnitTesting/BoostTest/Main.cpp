
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Master test for all unit tests based on Boost.Test framework.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tests/UnitTesting/BoostTest/UsulMath.h"
#include "Tests/UnitTesting/BoostTest/SceneGraph.h"
#include "Tests/UnitTesting/BoostTest/XmlTree.h"

#include "boost/test/included/unit_test_framework.hpp"


///////////////////////////////////////////////////////////////////////////////
//
//  Used for debugging.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    ~Init()
    {
      // Put breakpoint here to see program output.
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

boost::unit_test::test_suite *init_unit_test_suite ( int, char ** const )
{
  boost::unit_test::master_test_suite_t &ts ( boost::framework::master_test_suite() );
  ts.add ( BOOST_TEST_CASE ( &Tests::Usul::Math::test001 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::Usul::Math::test002 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::Usul::Math::test003 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::Usul::Math::test004 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::SceneGraph::test001 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::SceneGraph::test002 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::XmlTree::test001 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::XmlTree::test002 ) );
  ts.add ( BOOST_TEST_CASE ( &Tests::XmlTree::test003 ) );
  return 0;
}
