
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

#ifndef _TREE_REGISTRY_BUILDER_H_
#define _TREE_REGISTRY_BUILDER_H_

#include "Tree/Node.h"
#include "Tree/Registry/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

namespace Usul { namespace Registry { class Node; class Database; } }
namespace Tree { class Node; }


namespace Tree {
namespace Registry {


class TREE_REGISTRY_EXPORT Builder : 
  public Usul::Base::Referenced,
  public boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;

  USUL_REFERENCED_CLASS ( Builder );

  Builder();

  void        build ( const Tree::Node::RefPtr, Usul::Registry::Database & );
  void        build ( const Tree::Node::RefPtr, Usul::Registry::Node & );

protected:

  virtual ~Builder();

private:

  Usul::Threads::Check _threadCheck;
};


} // namespace Registry
} // namespace Tree


#endif // _TREE_REGISTRY_BUILDER_H_
