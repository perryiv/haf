
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Geometry shape node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_GEOMETRY_CLASS_H_
#define _SCENE_GRAPH_GEOMETRY_CLASS_H_

#include "SceneGraph/Nodes/Shapes/Shape.h"
#include "SceneGraph/Common/SharedVector.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "boost/function.hpp"


namespace SceneGraph {
namespace Nodes {
namespace Shapes {


class SCENE_GRAPH_EXPORT Geometry : public SceneGraph::Nodes::Shapes::Shape
{
public:

  SCENE_GRAPH_NODE ( Geometry, SceneGraph::Nodes::Shapes::Shape );

  // Type definitions for vertices, normals, colors, and texture coordinates.
  typedef Usul::Math::Vec3f Vertex;
  typedef Usul::Math::Vec3f Normal;
  typedef Usul::Math::Vec2f TexCoord;
  typedef Usul::Math::Vec4f Color;
  typedef SceneGraph::Common::SharedVector < Vertex > SharedVertices;
  typedef SceneGraph::Common::SharedVector < Normal > SharedNormals;
  typedef SceneGraph::Common::SharedVector < TexCoord > SharedTexCoords;
  typedef SceneGraph::Common::SharedVector < Color > SharedColors;
  typedef SharedVertices::Vector Vertices;
  typedef SharedNormals::Vector Normals;
  typedef SharedTexCoords::Vector TexCoords;
  typedef SharedColors::Vector Colors;
  typedef Usul::Math::Matrix44d Matrix;

  // Primitive modes.
  enum Mode
  {
    POINTS = 0,
    LINE_STRIP,
    LINE_LOOP,
    LINES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLES,
    QUAD_STRIP,
    QUADS,
    POLYGON,
    INVALID_MODE
  };

  // Type definitions for indices.
  typedef std::vector < unsigned int > Indices;
  typedef std::pair < Mode, Indices > Primitive;
  typedef SceneGraph::Common::SharedVector < Primitive > SharedPrimitives;
  typedef SharedPrimitives::Vector Primitives;

  // Callback for sorting primitives.
  typedef boost::function3
  <
    void, 
    const Matrix &, 
    Geometry &, 
    SharedPrimitives::RefPtr &
  >
  SortPrimitivesCallback;

  // Construction
  Geometry();

  // Set/get colors.
  const SharedColors::RefPtr      colorsGet() const { return _colors; }
  SharedColors::RefPtr            colorsGet()       { return _colors; }
  void                            colorsSet ( SharedColors::RefPtr );

  // Get a functor that sorts primitives back-to-front.
  static SortPrimitivesCallback   sortPrimitivesBackToFront();

  // Get/set the functor called to sort the primitives.
  SortPrimitivesCallback          sortPrimitivesCallbackGet() const;
  void                            sortPrimitivesCallbackSet ( SortPrimitivesCallback );

  // Set/get normals.
  const SharedNormals::RefPtr     normalsGet() const { return _normals; }
  SharedNormals::RefPtr           normalsGet()       { return _normals; }
  void                            normalsSet ( SharedNormals::RefPtr );

  // Add/get/set primitives.
  void                            primitiveAdd ( Mode, unsigned int first, unsigned int count );
  void                            primitiveAdd ( Mode, const Indices & );
  void                            primitiveAdd ( const Primitive & );
  const SharedPrimitives::RefPtr  primitivesGet() const { return _primitives; }
  SharedPrimitives::RefPtr        primitivesGet()       { return _primitives; }
  void                            primitivesSet ( SharedPrimitives::RefPtr );

  // Set/get texture coordinates.
  const SharedTexCoords::RefPtr   texCoordsGet() const { return _texCoords; }
  SharedTexCoords::RefPtr         texCoordsGet()       { return _texCoords; }
  void                            texCoordsSet ( SharedTexCoords::RefPtr );

  // Set/get vertices.
  const SharedVertices::RefPtr    verticesGet() const { return _vertices; }
  SharedVertices::RefPtr          verticesGet()       { return _vertices; }
  void                            verticesSet ( SharedVertices::RefPtr );

protected:

  // Use reference counting.
  virtual ~Geometry();

  virtual void                    _updateBoundingBox();
  virtual void                    _updateBoundingSphere();

private:

  Usul::Atomic::Object < SharedVertices::RefPtr > _vertices;
  Usul::Atomic::Object < SharedNormals::RefPtr > _normals;
  Usul::Atomic::Object < SharedTexCoords::RefPtr > _texCoords;
  Usul::Atomic::Object < SharedColors::RefPtr > _colors;
  Usul::Atomic::Object < SharedPrimitives::RefPtr > _primitives;
  Usul::Atomic::Object < SortPrimitivesCallback > _sortPrimitivesCallback;
};


} // namespace Shapes
} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_GEOMETRY_CLASS_H_
