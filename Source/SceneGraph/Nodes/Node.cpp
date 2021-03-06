
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The node class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Nodes/Node.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/NoThrow.h"

#include <algorithm>

using namespace SceneGraph::Nodes;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::Node() : BaseClass(),
  _flags ( DIRTY_BOUNDS | CONTRIBUTE_TO_BOUNDS ),
  _bSphere ( BoundingSphere ( false, BoundingSphere::second_type() ) ),
  _bBox ( BoundingBox() ),
  _parents()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
  USUL_TRY_BLOCK
  {
    // This will likely have elements because of the way Group and other 
    // classes destroy themselves. We could, alternatively, have Group 
    // remove itself as the parent of all its children, but that will 
    // take unnecessary cpu time.
    _parents.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "8666478010" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

Node::BoundingSphere Node::boundingSphereGet() const
{
  // If we're not contributing to the bounds then return invalid sphere.
  if ( false == this->contributeToBoundsGet() )
  {
    return BoundingSphere ( false, BoundingSphere::second_type() );
  }

  // If we are dirty then update first.
  if ( true == this->isDirtyBounds() )
  {
    Node *me ( const_cast < Node * > ( this ) );
    if ( 0x0 != me )
    {
      me->_updateBoundingSphere();
    }
  }

  // Return the bounds.
  return _bSphere;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Node::boundingSphereSet ( const BoundingSphere &b )
{
  _bSphere = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

Node::BoundingBox Node::boundingBoxGet() const
{
  // If we're not contributing to the bounds then return invalid box.
  if ( false == this->contributeToBoundsGet() )
  {
    return BoundingBox();
  }

  // If we are dirty then update first.
  if ( true == this->isDirtyBounds() )
  {
    Node *me ( const_cast < Node * > ( this ) );
    if ( 0x0 != me )
    {
      me->_updateBoundingBox();
    }
  }

  // Return the bounds.
  return _bBox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Node::boundingBoxSet ( const BoundingBox &b )
{
  _bBox = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag.
//
///////////////////////////////////////////////////////////////////////////////

void Node::contributeToBoundsSet ( bool state )
{
  this->_flagsSet ( CONTRIBUTE_TO_BOUNDS, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Node::contributeToBoundsGet() const
{
  return this->_flagsGet ( CONTRIBUTE_TO_BOUNDS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding sphere dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool Node::isDirtyBounds() const
{
  return this->_flagsGet ( DIRTY_BOUNDS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Node::dirtyBounds ( bool state, bool notifyParents )
{
  this->_flagsSet ( DIRTY_BOUNDS, state );

  if ( true == notifyParents )
  {
    // Have to lock rather than copy because it is a container of raw pointers.
    Guard guard ( _parents.mutex() );
    Parents &p ( _parents.getReference() );
    typedef Parents::iterator Itr;
    for ( Itr i = p.begin(); i != p.end(); ++i )
    {
      Node *node ( *i );
      if ( 0x0 != node )
      {
        node->dirtyBounds ( state, notifyParents );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Node::_flagsGet() const
{
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the state of the flags.
//
///////////////////////////////////////////////////////////////////////////////

bool Node::_flagsGet ( unsigned int f ) const
{
  return Usul::Bits::has ( this->_flagsGet(), f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Node::_flagsSet ( unsigned int f )
{
  _flags = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Node::_flagsSet ( unsigned int f, bool state )
{
  this->_flagsSet ( Usul::Bits::set ( this->_flagsGet(), f, state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a parent.
//
///////////////////////////////////////////////////////////////////////////////

void Node::parentAdd ( Node *parent )
{
  if ( 0x0 != parent )
  {
    Guard guard ( _parents.mutex() );
    Parents &p ( _parents.getReference() );
    p.insert ( p.end(), parent );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a parent.
//
///////////////////////////////////////////////////////////////////////////////

void Node::parentRemove ( Node *parent )
{
  if ( 0x0 != parent )
  {
    Guard guard ( _parents.mutex() );
    Parents &p ( _parents.getReference() );
    p.erase ( std::find ( p.begin(), p.end(), parent ) );
  }
}
