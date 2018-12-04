
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The transform group.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Nodes/Groups/Transform.h"

#include "Usul/Functions/NoThrow.h"

#include "boost/bind.hpp"

using namespace SceneGraph::Nodes::Groups;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructors.
//
///////////////////////////////////////////////////////////////////////////////

Transform::Transform() : BaseClass(),
  _matrix ( new SharedMatrix ( Matrix::getIdentity() ) )
{
}
Transform::Transform ( const Matrix &m ) : BaseClass(),
  _matrix ( new SharedMatrix ( m ) )
{
}
Transform::Transform ( SharedMatrix::RefPtr m ) : BaseClass(),
  _matrix ( m )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Transform::~Transform()
{
  Usul::Functions::noThrow ( boost::bind ( &Transform::_destroy, this ), "7265855690" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::_destroy()
{
  _matrix = SharedMatrix::RefPtr();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::matrixSet ( const Matrix &m )
{
  _matrix = new SharedMatrix ( m );
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix.
//
///////////////////////////////////////////////////////////////////////////////

Transform::Matrix Transform::matrixGet() const
{
  SharedMatrix::RefPtr m ( _matrix );
  return ( ( true == m.valid() ) ? m->get() : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::sharedMatrixSet ( SharedMatrix::RefPtr m )
{
  _matrix = m;
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix.
//
///////////////////////////////////////////////////////////////////////////////

Transform::SharedMatrix::RefPtr Transform::sharedMatrixGet()
{
  return _matrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::_updateBoundingSphere()
{
  // Do this first.
  BaseClass::_updateBoundingSphere();

  // This node's bounding sphere was just set by base class.
  const BoundingSphere local ( this->boundingSphereGet() );

  // Handle invalid bounds.
  if ( false == local.first )
  {
    return;
  }

  // Change the bounds by transforming the center.
  this->boundingSphereSet ( BoundingSphere ( true, BoundingSphere::second_type 
    ( this->matrixGet() * local.second.center(), local.second.radius() ) ) );

  // Our bounds are no longer dirty.
  this->dirtyBounds ( false, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::_updateBoundingBox()
{
  // Update bounds in local space.
  BaseClass::_updateBoundingBox();

  // Get the local-space bounds.
  const BoundingBox local ( this->boundingBoxGet() );

  // Handle invalid bounds.
  if ( false == local.valid() )
  {
    return;
  }

  // Transform the eight corners and making a new box.
  this->boundingBoxSet ( local.transform ( this->matrixGet() ) );

  // Our bounds are no longer dirty.
  this->dirtyBounds ( false, false );
}
