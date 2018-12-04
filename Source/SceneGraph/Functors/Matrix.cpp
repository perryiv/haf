
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Matrix functors.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Functors/Matrix.h"
#include "SceneGraph/Interfaces/INodeGet.h"

using namespace SceneGraph::Functors;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MatrixGet, MatrixGet::BaseClass );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MatrixReturn, MatrixReturn::BaseClass );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Perspective, Perspective::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

MatrixGet::MatrixGet() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

MatrixGet::~MatrixGet()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MatrixGet::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case SceneGraph::Interfaces::IMatrixGet::IID:
    return static_cast < SceneGraph::Interfaces::IMatrixGet * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MatrixReturn::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case SceneGraph::Interfaces::ILookAtAll::IID:
    return static_cast < SceneGraph::Interfaces::ILookAtAll * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Perspective::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case SceneGraph::Interfaces::IResizeNotify::IID:
    return static_cast < SceneGraph::Interfaces::IResizeNotify * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the matrix that views everything.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixReturn::lookAtAll ( IUnknown::RefPtr unknown )
{
  // Get needed interface.
  typedef SceneGraph::Interfaces::INodeGet INodeGet;
  INodeGet::QueryPtr ng ( unknown );
  if ( false == ng.valid() )
    return;

  // Get the scene.
  typedef SceneGraph::Nodes::Node Node;
  Node::RefPtr node ( ng->nodeGet() );
  if ( false == node.valid() )
    return;

  // Get the scene's bounding box.
  const Node::BoundingBox box ( node->boundingBoxGet() );
  if ( false == box.valid() )
    return;

  // Make a bounding sphere around the box.
  typedef Node::BoundingSphere::second_type Sphere;
  const Sphere sphere ( Sphere::bounds ( box.minimum(), box.maximum() ) );

  // Calculate the matrix.
  const double radius ( sphere.radius() );
  Usul::Math::Vec3d center ( sphere.center() );
  Usul::Math::Vec3d eye ( center[0], center[1], center[2] + radius * 2.0 );
  Usul::Math::Vec3d up ( 0, 1, 0 );
  _m = Matrix::makeLookAt<Matrix> ( eye, center, up );
}
