
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single element to be drawn.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_DRAW_ELEMENT_CLASS_H_
#define _SCENE_GRAPH_DRAW_ELEMENT_CLASS_H_

#include "SceneGraph/Common/SharedMatrix.h"
#include "SceneGraph/Nodes/Shapes/Shape.h"
#include "SceneGraph/State/Container.h"


namespace SceneGraph {
namespace Draw {


class SCENE_GRAPH_EXPORT Element : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( Element, SceneGraph::Base::Object );
  typedef SceneGraph::Nodes::Shapes::Shape Shape;
  typedef SceneGraph::State::Container StateContainer;
  typedef Usul::Math::Matrix44d Matrix;

  // Construction
  Element ( Shape::RefPtr, StateContainer::RefPtr, 
            const Matrix &, int importance );

  // Get the importance.
  int                       importance() const;

  // Get the matrix.
  const Matrix &            matrix() const;

  // Get the shape.
  Shape::RefPtr             shape();

  // Get the state-container.
  StateContainer::RefPtr    stateContainer();

protected:

  // Use reference counting.
  virtual ~Element();

private:

  Shape::RefPtr _shape;
  StateContainer::RefPtr _stateContainer;
  const Matrix _matrix;
  int _importance;
};


} // namespace Draw
} // namespace SceneGraph


#endif // _SCENE_GRAPH_DRAW_ELEMENT_CLASS_H_
