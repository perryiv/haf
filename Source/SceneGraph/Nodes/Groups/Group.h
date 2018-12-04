
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_GROUP_CLASS_H_
#define _SCENE_GRAPH_GROUP_CLASS_H_

#include "SceneGraph/Common/Enum.h"
#include "SceneGraph/Nodes/Node.h"

#include "Usul/Atomic/Container.h"


namespace SceneGraph {
namespace Nodes {
namespace Groups {


class SCENE_GRAPH_EXPORT Group : public SceneGraph::Nodes::Node
{
public:

  SCENE_GRAPH_NODE ( Group, Node );

  typedef SCENE_GRAPH_GROUP_CONTAINER_TYPE Nodes;
  typedef SceneGraph::Nodes::Node Node;
  typedef SceneGraph::Common::VisitMode VisitMode;

  // Construction.
  Group();

  // Pass the visitor to the nodes.
  void                          accept ( SceneGraph::Visitors::Visitor &, VisitMode::Mode );

  // Append a node.
  void                          append ( Node::RefPtr );

  // Return the child at the given index.
  Node::RefPtr                  at ( unsigned int ) const;

  // Are we empty?
  bool                          empty() const;

  // Find the index for the node. Returns the size if the node is not found.
  unsigned int                  find ( Node::RefPtr ) const;

  // Insert at the specified location, or append if the index is out of range.
  void                          insert ( unsigned int, Node::RefPtr );

  // Copy the child nodes.
  void                          nodes ( Nodes & ) const;

  // Prepend the node.
  void                          prepend ( Node::RefPtr );

  // Remove the node.
  void                          remove ( unsigned int );

  // Clear this node.
  void                          clear();

  // Return the number of nodes.
  unsigned int                  size() const;

protected:

  // Use reference counting.
  virtual ~Group();

  virtual void                  _updateBoundingBox();
  virtual void                  _updateBoundingSphere();

private:

  void                          _destroy();

  Usul::Atomic::Container < Nodes > _nodes;
};


} // namespace Groups
} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_GROUP_CLASS_H_
