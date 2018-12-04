
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

#ifndef _TREE_REGISTRY_VISITOR_H_
#define _TREE_REGISTRY_VISITOR_H_

#include "Tree/Node.h"
#include "Tree/Registry/Export.h"

#include "Usul/Registry/Visitor.h"
#include "Usul/Threads/Check.h"

#include <stack>
#include <string>


namespace Tree {
namespace Registry {


class TREE_REGISTRY_EXPORT Visitor : 
  public Usul::Registry::Visitor
{
public:

  typedef Usul::Registry::Visitor BaseClass;
  typedef std::stack < Tree::Node::RefPtr > NodeStack;

  USUL_REFERENCED_CLASS ( Visitor );

  // Construction
  Visitor();

  // Get the root.
  Tree::Node::RefPtr        root();

  // Visit the object.
  virtual void              visit ( Usul::Registry::Database * );
  virtual void              visit ( Usul::Registry::Node * );

protected:

  // Use reference counting.
  virtual ~Visitor();

private:

  Usul::Threads::Check _threadCheck;
  Tree::Node::RefPtr _root;
  NodeStack _stack;
};


} // namespace Registry
} // namespace Tree


#endif // _TREE_REGISTRY_VISITOR_H_
