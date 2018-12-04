
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all shape nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHAPE_CLASS_H_
#define _SCENE_GRAPH_SHAPE_CLASS_H_

#include "SceneGraph/Nodes/Node.h"
#include "SceneGraph/State/Container.h"

#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"


namespace SceneGraph {
namespace Nodes {
namespace Shapes {


class SCENE_GRAPH_EXPORT Shape : public SceneGraph::Nodes::Node
{
public:

  SCENE_GRAPH_NODE ( Shape, SceneGraph::Nodes::Node );

  typedef SceneGraph::State::Container StateContainer;
  typedef SceneGraph::Shaders::Shader Shader;

  // Get/set the draw-list index.
  int                           drawListIndexGet() const;
  void                          drawListIndexSet ( int );

  // Set/get the importance.
  void                          importanceSet ( int );
  int                           importanceGet() const;

  // Get/set the state-container.
  StateContainer::RefPtr        stateContainer ( bool createIfNeeded = true );
  void                          stateContainer ( StateContainer::RefPtr );

protected:

  // Construction.
  Shape();

  // Use reference counting.
  virtual ~Shape();

private:

  Usul::Atomic::Integer < int > _drawListIndex;
  Usul::Atomic::Integer < int > _importance;
  Usul::Atomic::Object < StateContainer::RefPtr > _stateContainer;
};


} // namespace Shapes
} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHAPE_CLASS_H_
