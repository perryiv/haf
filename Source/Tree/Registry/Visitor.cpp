
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor class that builds a tree from Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/Registry/Visitor.h"

#include "Usul/Registry/Database.h"
#include "Usul/Scope/Caller.h"

#include "boost/bind.hpp"

#include <stdexcept>

using namespace Tree;
using namespace Tree::Registry;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Visitor() : BaseClass(),
  _threadCheck(),
  _root(),
  _stack()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
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
  _threadCheck.throwIfDifferentThread();

  // Check input.
  if ( 0x0 == db )
    return;

  // Check input.
  if ( false == db->root().valid() )
    return;

  // Check state.
  if ( true == _root.valid() )
    throw std::runtime_error ( "Error 1970572843: root node is not null" );

  // Make our root.
  _root = Node::RefPtr ( new Node ( db->root()->name() ) );

  // Push root onto stack of current nodes, and pop it off when we return.
  Usul::Scope::Caller::RefPtr pushPop ( Usul::Scope::makeCaller (
    boost::bind ( &NodeStack::push, boost::ref ( _stack ), _root ), 
    boost::bind ( &NodeStack::pop, boost::ref ( _stack ) ) ) );

  // Visit root's children.
  BaseClass::visit ( db );

  // Because of the way the registry database has one node, the root 
  // always ends up with a single child node with the same name, and then 
  // multiple children under that. Here we put the single child's children 
  // directly under the root.
  if ( 1 == _root->numChildren() )
  {
    typedef Tree::Node::Children Children;

    // Get the root's children. There should only be one.
    Children rc ( _root->children() );
    if ( 1 == rc.size() )
    {
      // Get the first child and make sure it is valid.
      Tree::Node::RefPtr child ( rc.front() );
      if ( true == child.valid() )
      {
        // Set the root's children to be the first child's children.
        _root->children ( child->children() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Usul::Registry::Node *n )
{
  _threadCheck.throwIfDifferentThread();

  if ( 0x0 == n )
    return;

  // Make new node.
  Tree::Node::RefPtr node ( new Tree::Node 
    ( n->name(), n->get ( std::string() ) ) );

  // Add it to the current node.
  Tree::Node::RefPtr current ( _stack.top() );
  if ( true == current.valid() )
  {
    current->append ( node );
  }

  // Push node onto stack of current nodes, and pop it off when we return.
  Usul::Scope::Caller::RefPtr pushPop ( Usul::Scope::makeCaller (
    boost::bind ( &NodeStack::push, boost::ref ( _stack ), node ), 
    boost::bind ( &NodeStack::pop, boost::ref ( _stack ) ) ) );

  // Call base class to visit the children.
  BaseClass::visit ( n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root node.
//
///////////////////////////////////////////////////////////////////////////////

Tree::Node::RefPtr Visitor::root()
{
  _threadCheck.throwIfDifferentThread();
  return _root;
}
