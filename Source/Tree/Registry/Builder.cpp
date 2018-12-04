
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that traverses a tree and builds Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/Registry/Builder.h"

#include "Usul/Registry/Database.h"

using namespace Tree;
using namespace Tree::Registry;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Builder::Builder() : BaseClass(),
  _threadCheck()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Builder::~Builder()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add nodes to the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const Tree::Node::RefPtr xn, Usul::Registry::Node &rn )
  {
    typedef Tree::Node::Children Children;
    typedef Children::const_iterator Itr;

    if ( false == xn.valid() )
      return;

    const Children children ( xn->children() );
    for ( Itr i = children.begin(); i != children.end(); ++i )
    {
      const Tree::Node::RefPtr child ( *i );
      if ( true == child.valid() )
      {
        Usul::Registry::Node &node ( rn[child->name()] );
        node.set ( child->value() );
        Helper::traverse ( child, node );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the registry.
//
///////////////////////////////////////////////////////////////////////////////

void Builder::build ( const Tree::Node::RefPtr node, Usul::Registry::Node &reg )
{
  _threadCheck.throwIfDifferentThread();

  if ( true == node.valid() )
  {
    Helper::traverse ( node, reg );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the registry.
//
///////////////////////////////////////////////////////////////////////////////

void Builder::build ( const Tree::Node::RefPtr node, Usul::Registry::Database &db )
{
  _threadCheck.throwIfDifferentThread();

  if ( true == node.valid() )
  {
    // Get registry's root node.
    Usul::Registry::Node::RefPtr root ( db.root() );
    if ( true == root.valid() )
    {
      // Set registry's name and call other function.
      root->name ( node->name() );
      Builder::build ( node, *root );
    }
  }
}
