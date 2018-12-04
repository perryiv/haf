
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node representing a line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_LINE_CLASS_H_
#define _SCENE_GRAPH_LINE_CLASS_H_

#include "SceneGraph/Nodes/Shapes/Shape.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Math/Vector3.h"


namespace SceneGraph {
namespace Nodes {
namespace Shapes {


class SCENE_GRAPH_EXPORT Line : public SceneGraph::Nodes::Shapes::Shape
{
public:

  SCENE_GRAPH_NODE ( Line, SceneGraph::Nodes::Shapes::Shape );
  typedef Usul::Math::Vec3d Vector;

  // Construction.
  Line();
  Line ( const Vector &start, const Vector &end );

  // Set/get the end point.
  void                          endSet ( const Vector & );
  Vector                        endGet() const;

  // Set/get the start point.
  void                          startSet ( const Vector & );
  Vector                        startGet() const;

protected:

  // Use reference counting.
  virtual ~Line();

  virtual void                  _updateBoundingBox();
  virtual void                  _updateBoundingSphere();

private:

  Usul::Atomic::Object < Vector > _start;
  Usul::Atomic::Object < Vector > _end;
};


} // namespace Shapes
} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_LINE_CLASS_H_
