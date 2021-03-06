
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Registry/Visitor.h"
#include "Usul/Registry/Database.h"

using namespace Usul::Registry;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Visitor() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::~Visitor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry database.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Usul::Registry::Database *db )
{
  if ( 0x0 != db )
  {
    // Make sure node does not get deleted.
    Node::NoDeleteRefPtr root ( db->root() );

    // Visit the root node.
    this->visit ( root.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Usul::Registry::Node *node )
{
  typedef Usul::Registry::Node::Children Children;
  typedef Children::iterator Itr;

  if ( 0x0 != node )
  {
    // Copy sequence of smart-pointers to the children.
    Children kids ( node->children() );

    for ( Itr i = kids.begin(); i != kids.end(); ++i )
    {
      Usul::Registry::Node::RefPtr node ( i->second );
      if ( true == node.valid() )
      {
        node->accept ( this );
      }
    }
  }
}
