
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Body.h"
#include "Minerva/Document/Constants.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Scope/Caller.h"

#include "boost/bind.hpp"

#include <algorithm>

using namespace Minerva;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Body, BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body() : BaseClass(),
  _root ( new SceneGraph::Nodes::Transform ),
  _ellipse ( Ellipse ( Constants::WGS_84_RADIUS_EQUATOR, Constants::WGS_84_RADIUS_POLAR ) ),
  _topTiles(),
  _availableTiles(),
  _meshSize ( MeshSize ( 32, 32 ) )
{
  typedef Transform::Matrix Matrix;
  typedef Usul::Math::Vec3d Vec3d;

  // Don't allow reference counting to delete us.
  Usul::Scope::Caller::RefPtr unref ( Usul::Scope::makeCaller ( 
    boost::bind ( &Body::ref, this ),
    boost::bind ( &Body::unref, this, false ) ) );

  // Typical lon-lat-elev -> x-y-z conversions make the body's poles on the 
  // positive and negative z-axis. We want them on the top and bottom (y-axis).
  Transform::RefPtr &root ( _root.getReference() );
  root->matrixSet ( Matrix::rotation ( Usul::Math::PI_OVER_2, Vec3d ( -1, 0, 0 ) ) );

  // Need local scope.
  {
    typedef Usul::Interfaces::IUnknown IUnknown;
    IUnknown::QueryPtr me ( this );
    Tile::RefPtr parent;

    // Extents for the top-level tiles.
    const Extents we ( -180, -90,   0, 90 );
    const Extents ee (    0, -90, 180, 90 );

    // Calculate the mesh sizes.
    const MeshSize msw ( this->meshSize ( we ) );
    const MeshSize mse ( this->meshSize ( ee ) );

    // Add two top-level tiles.
    Tile::RefPtr western ( new Tile ( parent, me, 0, we, msw ) );
    Tile::RefPtr eastern ( new Tile ( parent, me, 0, ee, mse ) );

    // Save the tiles in our collection.
    _topTiles.getReference().push_back ( western );
    _topTiles.getReference().push_back ( eastern );

    // Attach their scenes.
    root->append ( western->scene() );
    root->append ( eastern->scene() );

    // Split the tiles.
    western->split ( 0 );
    eastern->split ( 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  Usul::Functions::noThrow ( boost::bind ( &Body::_destroy, this ), "5777627840" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  _root = Transform::RefPtr();

  // Destroy all top-tiles. This handles circular references.
  {
    TileVector tiles ( _topTiles.fetchAndStore ( TileVector() ) );
    for ( TileVector::iterator i = tiles.begin(); i != tiles.end(); ++i )
    {
      Tile::RefPtr tile ( *i );
      if ( true == tile.valid() )
      {
        tile->destroyObject();
      }
    }
  }

  // Destroy all the available-tiles.
  {
    TileList tiles ( _availableTiles.fetchAndStore ( TileList() ) );
    for ( TileList::iterator i = tiles.begin(); i != tiles.end(); ++i )
    {
      Tile::RefPtr tile ( *i );
      if ( true == tile.valid() )
      {
        tile->destroyObject();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Body::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IClearObject::IID:
    return static_cast < Usul::Interfaces::IClearObject * > ( this );
  case Usul::Interfaces::IDestroyObject::IID:
    return static_cast < Usul::Interfaces::IDestroyObject * > ( this );
  case Minerva::Interfaces::IBodyGet::IID:
    return static_cast < Minerva::Interfaces::IBodyGet * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this body.
//
///////////////////////////////////////////////////////////////////////////////

void Body::destroyObject()
{
  this->_destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this body.
//
///////////////////////////////////////////////////////////////////////////////

void Body::clearObject()
{
  // Clear all the available-tiles. Do not replace member.
  {
    TileList tiles ( _availableTiles );
    for ( TileList::iterator i = tiles.begin(); i != tiles.end(); ++i )
    {
      Tile::RefPtr tile ( *i );
      if ( true == tile.valid() )
      {
        tile->clearObject();
      }
    }
  }

  // Clear all top-tiles.
  {
    TileVector tiles ( _topTiles.fetchAndStore ( TileVector() ) );
    for ( TileVector::iterator i = tiles.begin(); i != tiles.end(); ++i )
    {
      Tile::RefPtr tile ( *i );
      if ( true == tile.valid() )
      {
        tile->clearObject();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the scene visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Body::accept ( SceneVisitor &visitor )
{
  Transform::RefPtr root ( _root );
  if ( true == root.valid() )
  {
    root->accept ( visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the ellipse.
//
///////////////////////////////////////////////////////////////////////////////

Body::Ellipse Body::ellipse() const
{
  return _ellipse;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the scene.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraph::Nodes::Transform::RefPtr Body::scene()
{
  return _root;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

Body::MeshSize Body::meshSize ( const Extents &extents ) const
{
  // Get default mesh size.
  const MeshSize ms ( this->meshSize() );

  // Get latitude as integer values.
  const long maxLat ( static_cast < long > ( extents.maxLat() ) );
  const long minLat ( static_cast < long > ( extents.minLat() ) );

  // If both ends are touching poles then return the default mesh.
  if ( ( 90 == maxLat ) && ( -90 == minLat ) )
    return ms;

  // Get the latitude closer to the equator.
  const double closerToEquatorLat ( Usul::Math::minimum 
    ( Usul::Math::absolute ( maxLat ), Usul::Math::absolute ( minLat ) ) );

  // The number of columns is a function of the latitude closer to the equator.
  const double fraction ( 1.0 - ( closerToEquatorLat / 90.0 ) );
  const double columns ( fraction * static_cast < double > ( ms[0] ) );

  // If we get to here then change the number of columns.
  return MeshSize ( ms[0], static_cast < MeshSize::value_type > ( columns ) );
}
