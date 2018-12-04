
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms that find elements.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_ALGORITHMS_FIND_H_
#define _TREE_ALGORITHMS_FIND_H_

#include "Tree/Predicates.h"

#include <string>


namespace Tree {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first node that passes the given predicate.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType, class Pred > 
inline typename NodeType::RefPtr findFirst
  ( const NodeType &node, Pred pred, bool traverse )
{
  typedef NodeType Node;
  typedef typename Node::RefPtr NodePtr;
  typedef typename Node::Children Children;

  // Loop through the children.
  const Children children ( node.children() );
  for ( Children::const_iterator i = children.begin(); i != children.end(); ++i )
  {
    // Get the child.
    Children::value_type child ( *i );

    // See if the child node passes the test. Note: do not check for null 
    // because that may be what the predicate does.
    if ( true == pred ( child ) )
    {
      return child;
    }

    // Traverse if we should.
    if ( ( true == traverse ) && ( true == child.valid() ) )
    {
      Children::value_type found ( Tree::Algorithms::findFirst ( *child, pred, traverse ) );
      if ( true == found.valid() )
      {
        return found;
      }
    }
  }

  // If we get to here then we did not find a child node.
  return NodePtr();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first node with the name.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType > inline typename NodeType::RefPtr findFirst
  ( const NodeType &node, const std::string &name, bool traverse )
{
  return Tree::Algorithms::findFirst ( node, Tree::Predicates::EqualName ( name ), traverse );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all nodes that passes the given predicate.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType, class Pred > 
inline typename NodeType::Children findAll
  ( const NodeType &node, Pred pred, bool traverse )
{
  typedef NodeType Node;
  typedef typename Node::RefPtr NodePtr;
  typedef typename Node::Children Children;

  // Initialize.
  Children answer;

  // Loop through the children.
  const Children children ( node.children() );
  for ( Children::const_iterator i = children.begin(); i != children.end(); ++i )
  {
    // Get the child.
    Children::value_type child ( *i );

    // See if the child node passes the test. Note: do not check for null 
    // because that may be what the predicate does.
    if ( true == pred ( child ) )
    {
      answer.push_back ( child );
    }

    // Traverse if we should.
    if ( ( true == traverse ) && ( true == child.valid() ) )
    {
      Children found ( Tree::Algorithms::findAll ( *child, pred, traverse ) );
      answer.insert ( answer.end(), found.begin(), found.end() );
    }
  }

  // Return the answer.
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the nodes with the name.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType > inline typename NodeType::Children findAll
  ( const NodeType &node, const std::string &name, bool traverse )
{
  return Tree::Algorithms::findAll ( node, Tree::Predicates::EqualName ( name ), traverse );
}


} // namespace Algorithms
} // namespace Tree


#endif // _TREE_ALGORITHMS_FIND_H_
