
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tile class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Tile.h"
#include "Minerva/Document/Body.h"
#include "Minerva/Document/Constants.h"

#include "SceneGraph/Nodes/Sphere.h"
#include "SceneGraph/State/Attributes/Attributes.h"
#include "SceneGraph/State/Attributes/Textures.h"

#include "Images/Core/Image2.h"
#include "Images/Interfaces/IImageRead.h"
#include "Images/Interfaces/IImageWrite.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Plugins/Manager.h"

#include "boost/bind.hpp"

#include <stdexcept>

using namespace Minerva;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Tile, BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the body from the unknown pointer.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  Minerva::Body::RefPtr getBody ( Usul::Interfaces::IUnknown::RefPtr unknown, bool throwIfNoBody )
  {
    Minerva::Interfaces::IBodyGet::QueryPtr bg ( unknown );
    Body::RefPtr body ( ( true == bg.valid() ) ? bg->bodyGet() : 0x0 );
    if ( ( false == body.valid() ) && ( true == throwIfNoBody ) )
    {
      throw std::runtime_error ( "Error 7726877460: could not convert unknown pointer to body" );
    }
    return body;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( Tile::RefPtr parent,
             IUnknown::RefPtr bodyPtr,
             unsigned int level,
             const Extents &extents,
             const MeshSize &meshSize ) : BaseClass(),
  _parent ( parent ),
  _body ( bodyPtr ),
  _level ( level ),
  _extents ( extents ),
  _root ( new Group ),
  _thisBranch ( new Transform ),
  _childBranch ( new Group ),
  _mesh(),
  _children()
{
  using namespace SceneGraph::Nodes;
  using namespace SceneGraph::State::Attributes;
  using namespace SceneGraph::State;
  typedef Transform::Matrix Matrix;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec2f Vec2f;

  const unsigned int numVerts ( meshSize[0] * meshSize[1] );

  Body::RefPtr body ( Helper::getBody ( _body, true ) );
  const Body::Ellipse ellipse ( body->ellipse() );

  Vec3d offset ( 0, 0, 0 );
  Mesh::SharedVertices vertices ( new Mesh::Vertices );
  Mesh::SharedNormals normals ( new Mesh::Normals );
  Mesh::SharedTexCoords texCoords ( new Mesh::TexCoords );
  vertices->reserve ( numVerts );
  normals->reserve ( numVerts );
  texCoords->reserve ( numVerts );

  for ( unsigned int i = 0; i < meshSize[0]; ++i )
  {
    const double u1 ( static_cast < double > ( i ) / ( meshSize[0] - 1 ) );
    const double lat ( extents.minLat() + u1 * ( extents.maxLat() - extents.minLat() ) );

    for ( unsigned int j = 0; j < meshSize[1]; ++j )
    {
      const double u2 ( static_cast < double > ( j ) / ( meshSize[1] - 1 ) );
      const double lon ( extents.minLon() + u2 * ( extents.maxLon() - extents.minLon() ) );
      const Vec3d xyz ( ellipse.toXYZ ( lon, lat, 0 ) );
#if 1
      if ( ( 0 == i ) && ( 0 == j ) )
      {
        offset = xyz;
      }
#endif
      const Vec3f v ( static_cast < float > ( xyz[0] - offset[0] ),
                      static_cast < float > ( xyz[1] - offset[1] ),
                      static_cast < float > ( xyz[2] - offset[2] ) );
      vertices->push_back ( v );

      const Vec3f n ( static_cast < float > ( xyz[0] ),
                      static_cast < float > ( xyz[1] ),
                      static_cast < float > ( xyz[2] ) );
      normals->push_back ( n.normalized() );

      texCoords->push_back ( Vec2f ( static_cast < float > ( u2 ), 
                                     static_cast < float > ( 1.0f - u1 ) ) );
    }
  }

  Mesh::RefPtr &mesh ( _mesh.getReference() );
  mesh = Mesh::RefPtr ( new Mesh ( meshSize ) );
  mesh->verticesSet ( vertices );
  mesh->normalsSet ( normals );
  mesh->texCoordsSet ( texCoords );

  StateSet::RefPtr state ( mesh->stateSet() );
  //state->attributeAdd ( new PolygonMode ( Face::BOTH, Mode::LINES ) );
  //state->attributeAdd ( new CullFace ( Face::BACK ) );
  ::srand ( reinterpret_cast < unsigned int > ( this ) );
  //state->attributeAdd ( new Color3 ( static_cast < double > ( ::rand() ) / RAND_MAX, static_cast < double > ( ::rand() ) / RAND_MAX, static_cast < double > ( ::rand() ) / RAND_MAX ) );

  Transform::RefPtr thisBranch ( _thisBranch );
  thisBranch->append ( mesh );
  //thisBranch->append ( new Sphere ( mesh->boundingSphereGet().second.radius() ) );
  thisBranch->matrixSet ( Matrix::translation ( offset ) );

  // Add the branch for this tile and not the children. 
  // Later, if we split, we do the opposite.
  _root.getReference()->append ( thisBranch );
  //_root.getReference()->drawBoundingSphereSet ( 2 == level );

#if 1

  typedef Images::Interfaces::IImageRead IImageRead;
  IImageRead::RefPtr reader ( dynamic_cast < IImageRead * > ( Usul::Plugins::Manager::instance().findFirst ( IImageRead::IID ).get() ) );
  if ( true == reader.valid() )
  {
    typedef Images::Core::Image2 < unsigned char > Image;
    std::string file ( "D:/home/perry/images/nasa/2004_07_blue_marble_2048x2048_west.jpg" );
    Image::RefPtr image ( dynamic_cast < Image * > ( reader->readImageFile ( file, IUnknown::RefPtr() ).get() ) );
    if ( true == image.valid() )
    {
      Texture2::RefPtr texture ( new Texture2 ( image ) );
      thisBranch->stateSet()->attributeAdd ( texture );
#if 0
      typedef Images::Interfaces::IImageWrite IImageWrite;
      Images::Interfaces::IImageWrite::QueryPtr writer ( reader );
      if ( true == writer.valid() )
      {
        Usul::Functions::noThrow ( boost::bind ( 
          &IImageWrite::writeImageFile, writer.get(), image, 
          file + ".jpg", IUnknown::RefPtr() ) );
      }
#endif
    }
  }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::~Tile()
{
  Usul::Functions::noThrow ( boost::bind ( &Tile::_destroy, this ), "2073008949" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_destroy()
{
  // Assign these members to null.
  _parent = Tile::RefPtr();
  _body = IUnknown::RefPtr();
  _root = Group::RefPtr();
  _thisBranch = Transform::RefPtr();
  _childBranch = Group::RefPtr();
  _mesh = Mesh::RefPtr();

  // Destroy all the child tiles. This handles circular references.
  TileVector children ( _children.fetchAndStore ( TileVector() ) );
  for ( TileVector::iterator i = children.begin(); i != children.end(); ++i )
  {
    Tile::RefPtr tile ( *i );
    if ( true == tile.valid() )
    {
      tile->destroyObject();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Tile::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IClearObject::IID:
    return static_cast < Usul::Interfaces::IClearObject * > ( this );
  case Usul::Interfaces::IDestroyObject::IID:
    return static_cast < Usul::Interfaces::IDestroyObject * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::destroyObject()
{
  this->_destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::clearObject()
{
  // Clear all the child tiles. This handles circular references.
  TileVector children ( _children.fetchAndStore ( TileVector() ) );
  for ( TileVector::iterator i = children.begin(); i != children.end(); ++i )
  {
    Tile::RefPtr tile ( *i );
    if ( true == tile.valid() )
    {
      tile->clearObject();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the extents.
//
///////////////////////////////////////////////////////////////////////////////

Tile::Extents Tile::extents() const
{
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::extents ( const Extents &e )
{
  _extents = e;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the scene.
//
///////////////////////////////////////////////////////////////////////////////

Tile::NodePtr Tile::scene()
{
  return NodePtr ( Group::RefPtr ( _root ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the level.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Tile::level() const
{
  return _level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split this tile into four children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::split ( unsigned int numTimes )
{
  // Handle splitting zero times.
  if ( 0 == numTimes )
    return;

  // Get the body.
  IUnknown::RefPtr ibody ( _body );
  Body::RefPtr body ( Helper::getBody ( ibody, true ) );

  // Get the new extents.
  const Extents extents ( this->extents() );
  Extents ll, lr, ul, ur;
  extents.split ( ll, lr, ul, ur );

  // Get new level.
  const unsigned int level ( this->level() + 1 );
  
  // Mesh sizes for child tiles.
  const MeshSize mll ( body->meshSize ( ll ) );
  const MeshSize mlr ( body->meshSize ( lr ) );
  const MeshSize mul ( body->meshSize ( ul ) );
  const MeshSize mur ( body->meshSize ( ur ) );

  // Make the tiles.
  Tile::RefPtr tll ( new Tile ( this, ibody, level, ll, mll ) );
  Tile::RefPtr tlr ( new Tile ( this, ibody, level, lr, mlr ) );
  Tile::RefPtr tul ( new Tile ( this, ibody, level, ul, mul ) );
  Tile::RefPtr tur ( new Tile ( this, ibody, level, ur, mur ) );

  // Set the children.
  {
    TileVector tiles;
    tiles.push_back ( tll );
    tiles.push_back ( tlr );
    tiles.push_back ( tul );
    tiles.push_back ( tur );
    Guard guard ( _children.mutex() );
    tiles.swap ( _children.getReference() );
  }

  // Add the new tiles' scenes.
  Group::RefPtr childBranch ( _childBranch );
  if ( false == childBranch.valid() )
    return;
  childBranch->append ( tll->scene() );
  childBranch->append ( tlr->scene() );
  childBranch->append ( tul->scene() );
  childBranch->append ( tur->scene() );

  // Remove this tile's branch from the root and add the one for children.
  {
    Guard guard ( _root.mutex() );
    Group::RefPtr &root ( _root.getReference() );
    if ( false == root.valid() )
      return;
    root->remove ( root->find ( Transform::RefPtr ( _thisBranch ) ) );
    root->remove ( root->find ( childBranch ) );
    root->append ( childBranch );
  }

  // Have each tile split again.
  --numTimes;
  tll->split ( numTimes );
  tlr->split ( numTimes );
  tul->split ( numTimes );
  tur->split ( numTimes );
}
