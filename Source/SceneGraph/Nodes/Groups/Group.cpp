
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

#include "SceneGraph/Nodes/Groups/Group.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"

#include <stdexcept>
#include <vector>

using namespace SceneGraph::Nodes::Groups;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group() : BaseClass(),
  _nodes()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
  Usul::Functions::noThrow ( boost::bind ( &Group::_destroy, this ), "1770715368" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_destroy()
{
  _nodes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the nodes.
//
///////////////////////////////////////////////////////////////////////////////

void Group::clear()
{
  _nodes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::insert ( unsigned int position, Node::RefPtr node ) 
{
  // Handle bad input.
  if ( ( false == node.valid() ) || ( node.get() == this ) )
    return;

  // Lock the nodes.
  Guard guard ( _nodes.mutex() );
  Nodes &nodes ( _nodes.getReference() );

  // Adjust the index if needed.
  const unsigned int numChildren ( nodes.size() );
  if ( position > numChildren )
  {
    position = numChildren;
  }

  // Figure out the iterator.
  Nodes::iterator i ( nodes.begin() );
  std::advance ( i, position );

  // Insert at the iterator.
  nodes.insert ( i, node );

  // We are a parent.
  node->parentAdd ( this );

  // We have dirty bounds.
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::append ( Node::RefPtr node ) 
{
  // Handle bad input.
  if ( ( false == node.valid() ) || ( node.get() == this ) )
    return;

  // Lock and append.
  Guard guard ( _nodes.mutex() );
  Nodes &nodes ( _nodes.getReference() );
  nodes.insert ( nodes.end(), node );

  // We are a parent.
  node->parentAdd ( this );

  // We have dirty bounds.
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::prepend ( Node::RefPtr node ) 
{
  // Handle bad input.
  if ( ( false == node.valid() ) || ( node.get() == this ) )
    return;

  // Lock and prepend.
  Guard guard ( _nodes.mutex() );
  Nodes &nodes ( _nodes.getReference() );
  nodes.insert ( nodes.begin(), node );

  // We are a parent.
  node->parentAdd ( this );

  // We have dirty bounds.
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the child.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Group::find ( Node::RefPtr node ) const
{
  // Lock and get shortcut.
  Guard guard ( _nodes.mutex() );
  const Nodes &nodes ( _nodes.getReference() );

  // Loop through the nodes.
  typedef Nodes::const_iterator Itr;
  unsigned int position ( 0 );
  for ( Itr i = nodes.begin(); i != nodes.end(); ++i )
  {
    if ( *i == node )
    {
      break;
    }
    ++position;
  }

  // Return the position, which is the size if we did not find the node.
  return position;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::remove ( unsigned int index ) 
{
  // Lock the nodes.
  Guard guard ( _nodes.mutex() );
  Nodes &nodes ( _nodes.getReference() );

  // Handle index out of range.
  if ( index >= nodes.size() )
    return;

  // Find the child's iterator.
  Nodes::iterator i ( nodes.begin() );
  std::advance ( i, index );

  // Punt if we're at the end.
  if ( nodes.end() == i )
    return;

  // Reference the child now.
  Node::RefPtr child ( *i );

  // Erase the child.
  nodes.erase ( i );

  // We are no longer a parent.
  if ( true == child.valid() )
    child->parentRemove ( this );

  // We have dirty bounds.
  this->dirtyBounds ( true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Group::size() const
{
  return _nodes.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Group::empty() const
{
  return _nodes.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraph::Nodes::Node::RefPtr Group::at ( unsigned int index ) const
{
  // Lock nodes.
  Guard guard ( _nodes.mutex() );
  const Nodes &nodes ( _nodes.getReference() );

  // Check index.
  if ( index >= nodes.size() )
  {
    throw std::range_error ( Usul::Strings::format 
      ( "Error 1040651709: index ", index, " out of range" ) );
  }

  // Figure out the iterator.
  Nodes::const_iterator i ( nodes.begin() );
  std::advance ( i, index );

  // Return the node.
  return *i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy the nodes.
//
///////////////////////////////////////////////////////////////////////////////

void Group::nodes ( Group::Nodes &to ) const
{
  // Lock and copy.
  Guard guard ( _nodes.mutex() );
  const Nodes &from ( _nodes.getReference() );
  to.assign ( from.begin(), from.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add the point.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class VectorType, class AverageType, class PointsType >
  inline void addPoint ( const VectorType &v, PointsType &points, AverageType &average )
  {
    points.push_back ( v );
    average += v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to calculate the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Group::BoundingSphere calculateBounds ( const Group::Nodes &nodes )
  {
    typedef Group::Nodes::const_iterator Itr;
    typedef Usul::Math::Sphered Sphered;
    typedef Sphered::Vector Vec3;
    typedef std::vector < Vec3 > Points;
    typedef Group::BoundingSphere BoundingSphere;

    // Handle no nodes.
    if ( true == nodes.empty() )
    {
      return BoundingSphere ( false, Sphered() );
    }

    // The list of bounding-box vertices.
    Points points;
    points.reserve ( nodes.size() * 8 );

    // This will be the average bounding-box vertex.
    Vec3 average ( 0, 0, 0 );

    // Loop through a copy of the nodes.
    for ( Itr i = nodes.begin(); i != nodes.end(); ++i )
    {
      const Group::Nodes::value_type &node ( *i );
      if ( true == node.valid() )
      {
        // Get child's bounding sphere.
        const BoundingSphere childBounds ( node->boundingSphereGet() );

        // If it's valid...
        if ( true == childBounds.first )
        {
          // Add the vertices of the bounding-box.
          const Vec3 mn ( childBounds.second.boxMin() );
          const Vec3 mx ( childBounds.second.boxMax() );

          // Add all eight corners to the collection and the average.
          Helper::addPoint ( Vec3 ( mn[0], mn[1], mn[2] ), points, average );
          Helper::addPoint ( Vec3 ( mx[0], mn[1], mn[2] ), points, average );
          Helper::addPoint ( Vec3 ( mx[0], mx[1], mn[2] ), points, average );
          Helper::addPoint ( Vec3 ( mn[0], mx[1], mn[2] ), points, average );
          Helper::addPoint ( Vec3 ( mn[0], mn[1], mx[2] ), points, average );
          Helper::addPoint ( Vec3 ( mx[0], mn[1], mx[2] ), points, average );
          Helper::addPoint ( Vec3 ( mx[0], mx[1], mx[2] ), points, average );
          Helper::addPoint ( Vec3 ( mn[0], mx[1], mx[2] ), points, average );
        }
      }
    }

    // Handle no valid bounding spheres.
    if ( true == points.empty() )
    {
      return BoundingSphere ( false, Sphered() );
    }

    // Calculate the average point.
    average /= points.size();

    // Determine the maximum distance from the average (center) to any point.
    double maxDist ( 0 );
    unsigned int maxPtIndex ( 0 );
    for ( unsigned int i = 0; i < points.size(); ++i )
    {
      const Vec3 &pt ( points[i] );
      const double dist ( pt.distanceSquared ( average ) );
      if ( dist > maxDist )
      {
        maxDist = dist;
        maxPtIndex = i;
      }
    }
    maxDist = average.distance ( points.at ( maxPtIndex ) );

    // Return new bounding sphere.
    return BoundingSphere ( true, Sphered ( average, maxDist ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_updateBoundingSphere()
{
  typedef Usul::Math::Sphered Sphered;

  // Copy the nodes.
  Nodes nodes ( _nodes );

  // Initialize the new bounds.
  BoundingSphere ourBounds ( false, Sphered() );

  // The method used depends on the number of children.
  switch ( nodes.size() )
  {
    case 0:
      break;

    case 1:
    {
      Node::RefPtr node ( nodes.front() );
      if ( true == node.valid() )
      {
        ourBounds = node->boundingSphereGet();
      }
      break;
    }

    case 2:
    {
      BoundingSphere s1 ( false, Sphered() );
      BoundingSphere s2 ( false, Sphered() );
      Node::RefPtr node ( nodes.front() );
      if ( true == node.valid() )
      {
        s1 = node->boundingSphereGet();
      }
      node = nodes.back();
      if ( true == node.valid() )
      {
        s2 = node->boundingSphereGet();
      }
      if ( ( true == s1.first ) && ( true == s2.first ) )
      {
        ourBounds = BoundingSphere ( true, 
          Sphered::bounds ( s1.second, s2.second ) );
      }
      else
      {
        ourBounds = ( ( true == s1.first ) ? s1 : s2 );
      }
      break;
    }

    default:
      ourBounds = Helper::calculateBounds ( nodes );
      break;
  }

  // Set new bounding sphere.
  this->boundingSphereSet ( ourBounds );
  this->dirtyBounds ( false, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_updateBoundingBox()
{
  // Copy the node pointers.
  Nodes nodes ( _nodes );

  // Initialize the new bounds.
  BoundingBox bbox;

  // Loop through the nodes.
  typedef Group::Nodes::const_iterator Itr;
  for ( Itr i = nodes.begin(); i != nodes.end(); ++i )
  {
    const Group::Nodes::value_type &node ( *i );
    if ( true == node.valid() )
    {
      // Add child's bounding box.
      bbox = BoundingBox::bounds ( bbox, node->boundingBoxGet() );
    }
  }

  // Set new bounding box.
  this->boundingBoxSet ( bbox );
  this->dirtyBounds ( false, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for visiting the nodes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class V, class N > inline void accept ( V &v, N &n )
  {
    typedef typename N::iterator Itr;
    typedef typename N::value_type NodePtr;

    for ( Itr i = n.begin(); i != n.end(); ++i )
    {
      NodePtr &node ( *i );
      if ( true == node.valid() )
      {
        node->accept ( v );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pass the visitor to the nodes.
//
///////////////////////////////////////////////////////////////////////////////

void Group::accept ( SceneGraph::Visitors::Visitor &v, VisitMode::Mode mode )
{
  if ( VisitMode::COPY_CHILDREN == mode )
  {
    Nodes nodes ( _nodes );
    Helper::accept ( v, nodes );
  }
  else if ( VisitMode::LOCK_CHILDREN == mode )
  {
    Guard guard ( _nodes.mutex() );
    Helper::accept ( v, _nodes.getReference() );
  }
  else
  {
    throw std::runtime_error ( "Error 1738178140: visit mode not implemented" );
  }
}
