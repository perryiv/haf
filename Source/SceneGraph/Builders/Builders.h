
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Builder classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_BUILDERS_H_
#define _SCENE_GRAPH_BUILDERS_H_

#include "SceneGraph/Nodes/Shapes/Geometry.h"

#include "Usul/Algorithms/Sphere.h"
#include "Usul/Math/Vector3.h"


namespace SceneGraph {
namespace Builders {


///////////////////////////////////////////////////////////////////////////////
//
//  Sphere builder.
//
///////////////////////////////////////////////////////////////////////////////

struct Sphere
{
  typedef Usul::Math::Vec3f Center;
  typedef Center::value_type Radius;
  typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
  typedef Geometry::SharedVertices SharedVertices;
  typedef Geometry::SharedNormals SharedNormals;
  typedef Geometry::SharedColors SharedColors;
  typedef Geometry::Vertices Vertices;
  typedef Geometry::Colors Colors;
  typedef Geometry::Primitive Primitive;

  Sphere ( Radius r = 1, const Center &c = Center ( 0, 0, 0 ) ) : 
    _r ( r ), _c ( c )
  {
  }

  Geometry::RefPtr build ( unsigned int numSubdivisions = 1 )
  {
    // Generate vertices.
    Vertices v;
    Usul::Algorithms::sphere<Center::value_type> ( numSubdivisions, v );

    // The vertices are also the normals. Set them first.
    Geometry::RefPtr g ( new Geometry );
    g->normalsSet ( SharedNormals::RefPtr ( new SharedNormals  ( v ) ) );

    // Scale the vertices if we should.
    if ( 1 != _r )
    {
      for ( Vertices::iterator i = v.begin(); i != v.end(); ++i )
      {
        Vertices::value_type &v ( *i );
        v *= _r;
      }
    }

    // Offset the vertices if we should.
    if ( false == Center ( 0, 0, 0 ).equal ( _c ) )
    {
      for ( Vertices::iterator i = v.begin(); i != v.end(); ++i )
      {
        Vertices::value_type &v ( *i );
        v += _c;
      }
    }

    // Now set the vertices.
    g->verticesSet ( SharedVertices::RefPtr ( new SharedVertices ( v ) ) );

    // Add one collection of triangles.
    g->primitiveAdd ( Geometry::TRIANGLES, 0, v.size() );

    // Return the new geometry.
    return g;
  }

  const Center &center() const
  {
    return _c;
  }

  void center ( const Center &c )
  {
    _c = c;
  }

  Radius radius() const
  {
    return _r;
  }

  void radius ( Radius r )
  {
    _r = r;
  }

private:

  Radius _r;
  Center _c;
};


} // namespace Builders
} // namespace SceneGraph


#endif // _SCENE_GRAPH_BUILDERS_H_
