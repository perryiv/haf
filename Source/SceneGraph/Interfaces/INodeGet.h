
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to get a node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_NODE_GET_H_
#define _SCENE_GRAPH_INTERFACES_NODE_GET_H_

#include "Usul/Interfaces/IUnknown.h"

#include "SceneGraph/Nodes/Node.h"


namespace SceneGraph {
namespace Interfaces {


struct INodeGet : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef SceneGraph::Nodes::Node::RefPtr NodePtr;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INodeGet );

  // Id for this interface.
  enum { IID = 2145757481u };

  // Get the node.
  virtual NodePtr             nodeGet() = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_NODE_GET_H_
