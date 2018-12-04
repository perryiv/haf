
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

#include "SceneGraph/Nodes/Shapes/Geometry.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"


using namespace SceneGraph::Nodes::Shapes;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry() : BaseClass(),
  _vertices(),
  _normals(),
  _texCoords(),
  _colors(),
  _primitives(),
  _sortPrimitivesCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::~Geometry()
{
  USUL_TRY_BLOCK
  {
    // Done with these.
    _vertices = SharedVertices::RefPtr();
    _normals = SharedNormals::RefPtr();
    _texCoords = SharedTexCoords::RefPtr();
    _colors = SharedColors::RefPtr();
    _primitives = SharedPrimitives::RefPtr();
    _sortPrimitivesCallback = SortPrimitivesCallback();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "4013109395" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sort callback.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::sortPrimitivesCallbackSet ( SortPrimitivesCallback cb )
{
  _sortPrimitivesCallback = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the sort callback.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::SortPrimitivesCallback Geometry::sortPrimitivesCallbackGet() const
{
  // Needed to explicitely return the contained object for VC++ 2008.
  typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;
  Guard guard ( _sortPrimitivesCallback.mutex() );
  return _sortPrimitivesCallback.getReference();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for sorting triangles.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct SortTriangles
  {
    typedef Geometry::Matrix Matrix;
    typedef Geometry::Vertices Vertices;
    typedef Geometry::Vertex Vertex;
    typedef Usul::Math::Vector3<Matrix::value_type> Vec3;
    SortTriangles ( const Matrix &m, const Vertices &v ) : 
      _m ( m ), _v ( v )
    {
    }
    template < class Triangle > bool operator () 
      ( const Triangle &ta, const Triangle &tb )
    {
      // Get the average vertex for triangle a.
      const Vertex tav0 ( _v.at ( ta[0] ) );
      const Vertex tav1 ( _v.at ( ta[1] ) );
      const Vertex tav2 ( _v.at ( ta[2] ) );
      const Vertex tavm ( ( tav0 + tav1 + tav2 ) * 0.33f );

      // Get the average vertex for triangle b.
      const Vertex tbv0 ( _v.at ( tb[0] ) );
      const Vertex tbv1 ( _v.at ( tb[1] ) );
      const Vertex tbv2 ( _v.at ( tb[2] ) );
      const Vertex tbvm ( ( tbv0 + tbv1 + tbv2 ) * 0.33f );

      // Convert the average vertives to global space.
      const Vec3 vag ( _m * Vec3 ( tavm[0], tavm[1], tavm[2] ) );
      const Vec3 vbg ( _m * Vec3 ( tbvm[0], tbvm[1], tbvm[2] ) );

      // Return false if a is closer to origin that b.
      return ( vag.dot ( vag ) < vbg.dot ( vbg ) );
    }
  private:
    Matrix _m;
    const Vertices &_v;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for sorting the primitives.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void sortPrimitivesBackToFront ( 
    const Geometry::Matrix &m, 
    Geometry &g, 
    Geometry::SharedPrimitives::RefPtr &target )
  {
    typedef Geometry::SharedPrimitives SharedPrimitives;
    typedef Geometry::Primitives Primitives;
    typedef Primitives::iterator Itr;
    typedef Geometry::Primitive Primitive;
    typedef Geometry::Mode Mode;
    typedef Geometry::Indices Indices;
    typedef SharedPrimitives::AtomicVector AtomicPrimitives;
    typedef Geometry::SharedVertices SharedVertices;
    typedef SharedVertices::AtomicVector AtomicVertices;
    typedef Geometry::Vertices Vertices;
    typedef Usul::Threads::Guard < AtomicPrimitives::Mutex > Guard;
    typedef Usul::Math::Vec3ui Triangle;
    typedef std::vector < Triangle > Triangles;

    // Require a valid target.
    if ( false == target.valid() )
      return;

    // Get target primitives.
    AtomicPrimitives &atomicTarget ( target->get() );
    Guard guard1 ( atomicTarget.mutex() );
    Primitives &targetPrims ( atomicTarget.getReference() );

    // Clear the target primitives.
    targetPrims.clear();

    // Require shared primitives for source.
    SharedPrimitives::RefPtr source ( g.primitivesGet() );
    if ( false == source.valid() )
      return;

    // Get source primitives.
    AtomicPrimitives &atomicSource ( source->get() );
    Guard guard2 ( atomicSource.mutex() );
    Primitives &sourcePrims ( atomicSource.getReference() );

    // Get vertices.
    SharedVertices::RefPtr sv ( g.verticesGet() );
    if ( false == sv.valid() )
      return;
    AtomicVertices &atomicVertices ( sv->get() );
    Guard guard3 ( atomicVertices.mutex() );
    Vertices &vertices ( atomicVertices.getReference() );

    // Predicate used to sort triangles.
    Helper::SortTriangles pred ( m, vertices );

    // Declare triangles up here.
    Triangles triangles;

    // Loop through the primitives.
    for ( Itr i = sourcePrims.begin(); i != sourcePrims.end(); ++i )
    {
      // If the primitives is a set of triangles...
      const Primitive &sourcePrim ( *i );
      const unsigned int numVertices ( sourcePrim.second.size() );
      if ( ( Geometry::TRIANGLES == sourcePrim.first ) && ( 0 == numVertices % 3 ) )
      {
        // Make sequence of triangles.
        triangles.clear();
        const unsigned int numTriangles ( numVertices / 3 );
        triangles.reserve ( numTriangles );
        for ( unsigned int i = 0; i < numVertices; i += 3 )
        {
          triangles.push_back ( Triangle ( 
            sourcePrim.second.at ( i     ),
            sourcePrim.second.at ( i + 1 ),
            sourcePrim.second.at ( i + 2 ) ) );
        }

        // Sort triangles.
        std::sort ( triangles.begin(), triangles.end(), pred );

        // Copy sorted indices back to target primitive.
        targetPrims.push_back ( Primitive ( sourcePrim.first, Indices() ) );
        Indices &targetIndices ( targetPrims.back().second );
        for ( unsigned int j = 0; j < numTriangles; ++j )
        {
          const Triangle &t ( triangles.at ( j ) );
          targetIndices.push_back ( t[0] );
          targetIndices.push_back ( t[1] );
          targetIndices.push_back ( t[2] );
        }
      }

      // Otherwise, copy primitive as is.
      else
      {
        targetPrims.push_back ( sourcePrim );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a sort callback.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::SortPrimitivesCallback Geometry::sortPrimitivesBackToFront()
{
  return boost::bind ( &Helper::sortPrimitivesBackToFront, _1, _2, _3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitives.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::primitivesSet ( SharedPrimitives::RefPtr p )
{
  _primitives = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::primitiveAdd ( const Primitive &p )
{
  // Handle bad input.
  if ( ( Geometry::INVALID_MODE == p.first ) || ( true == p.second.empty() ) )
    return;

  // Get shared primitives.
  SharedPrimitives::RefPtr sp ( this->primitivesGet() );

  // Handle first time.
  if ( false == sp.valid() )
  {
    sp = SharedPrimitives::RefPtr ( new SharedPrimitives );
    this->primitivesSet ( sp );
  }

  // Get shortcuts.
  SharedPrimitives::AtomicVector &ap ( sp->get() );
  Guard guard ( ap.mutex() );
  Primitives &primitives ( ap.getReference() );

  // Add the primitive.
  primitives.push_back ( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::primitiveAdd ( Mode mode, const Indices &indices )
{
  this->primitiveAdd ( Primitive ( mode, indices ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return increasing values.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct Increment
  {
    Increment ( unsigned int first ) : _num ( first ){}
    unsigned int operator () ()
    {
      return _num++;
    }
  private:
    unsigned int _num;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::primitiveAdd ( Mode mode, unsigned int first, unsigned int count )
{
  // Generate the indices.
  Indices indices ( count );
  std::generate ( indices.begin(), indices.end(), Helper::Increment ( first ) );

  // Call the other function.
  this->primitiveAdd ( Primitive ( mode, indices ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::colorsSet ( SharedColors::RefPtr c )
{
  _colors = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::texCoordsSet ( SharedTexCoords::RefPtr t )
{
  _texCoords = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the normals.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::normalsSet ( SharedNormals::RefPtr n )
{
  _normals = n;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::verticesSet ( SharedVertices::RefPtr v )
{
  _vertices = v;
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::_updateBoundingSphere()
{
  // Get the shared vector of vertices.
  SharedVertices::RefPtr sv ( _vertices );
  if ( false == sv.valid() )
    return;
  const SharedVertices::AtomicVector av ( sv->get() );
  Guard guard ( av.mutex() );
  const Vertices &v ( av.getReference() );
  if ( true == v.empty() )
    return;

  // Find average vertex in double precision.
  Usul::Math::Vec3d da ( 0, 0, 0 );
  for ( Vertices::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    const Vertex &pt ( *i );
    da[0] += pt[0];
    da[1] += pt[1];
    da[2] += pt[2];
  }
  da /= v.size();

  // Convert to single precision.
  const Vertex average 
    ( static_cast < Vertex::value_type > ( da[0] ), 
      static_cast < Vertex::value_type > ( da[1] ),
      static_cast < Vertex::value_type > ( da[2] ) );

  // Find max distance between average and all others.
  float maxDist ( 0 );
  for ( Vertices::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    const Vertex &pt ( *i );
    const float dist ( average.distanceSquared ( pt ) );
    if ( dist > maxDist )
      maxDist = dist;
  }
  maxDist = ( ( maxDist > 0 ) ? Usul::Math::sqrt ( maxDist ) : 0 );

  // Set new bounding sphere.
  Usul::Math::Sphered sphere ( da, maxDist );
  this->boundingSphereSet ( BoundingSphere ( true, sphere ) );

  // No longer dirty.
  this->dirtyBounds ( false, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::_updateBoundingBox()
{
  // Get the shared vector of vertices.
  SharedVertices::RefPtr sv ( _vertices );
  if ( false == sv.valid() )
    return;
  const SharedVertices::AtomicVector av ( sv->get() );
  Guard guard ( av.mutex() );
  const Vertices &v ( av.getReference() );
  if ( true == v.empty() )
    return;

  // Initialize bounding box.
  BoundingBox bbox;

  // Loop through the vertices.
  for ( Vertices::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    // Grow the bounding box.
    const Vertex &pf ( *i );
    const BoundingBox::Vector pd ( pf[0], pf[1], pf[2] );
    bbox = BoundingBox::bounds ( bbox, pd );
  }

  // Set new bounding box.
  this->boundingBoxSet ( bbox );

  // No longer dirty.
  this->dirtyBounds ( false, false );
}
