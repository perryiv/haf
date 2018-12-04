
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

#ifndef _MINERVA_DOCUMENT_TILE_CLASS_H_
#define _MINERVA_DOCUMENT_TILE_CLASS_H_

#include "Minerva/Document/Extents.h"

#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/Nodes/Shapes/Mesh.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClearObject.h"
#include "Usul/Interfaces/IDestroyObject.h"
#include "Usul/Math/Vector2.h"

#include "boost/noncopyable.hpp"

#include <vector>


namespace Minerva {


class Tile : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IClearObject,
  public Usul::Interfaces::IDestroyObject
{
public:

  USUL_REFERENCED_CLASS ( Tile );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  typedef Usul::Base::Referenced BaseClass;
  typedef SceneGraph::Nodes::Groups::Transform Transform;
  typedef SceneGraph::Nodes::Groups::Group Group;
  typedef SceneGraph::Nodes::Shapes::Mesh Mesh;
  typedef Minerva::Extents < Usul::Math::Vec2d > Extents;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef std::vector < Tile::RefPtr > TileVector;
  typedef SceneGraph::Nodes::Node::RefPtr NodePtr;
  typedef Usul::Math::Vec2ui MeshSize;

  // Construction
  Tile ( Tile::RefPtr parent,
         IUnknown::RefPtr body,
         unsigned int level,
         const Extents &,
         const MeshSize & );

  // Usul::Interfaces::IClearObject
  virtual void                    clearObject();

  // Usul::Interfaces::IDestroyObject
  virtual void                    destroyObject();

  // Set/get the extents.
  void                            extents ( const Extents & );
  Extents                         extents() const;

  // Return the level.
  unsigned int                    level() const;

  // Get the scene.
  NodePtr                         scene();

  // Split this tile into four children. Pass a number greater than one to
  // recursively split further.
  void                            split ( unsigned int numTimes = 1 );

protected:

  // Use reference counting.
  virtual ~Tile();

  void                            _destroy();

private:

  Usul::Atomic::Object < Tile::RefPtr > _parent;
  Usul::Atomic::Object < IUnknown::RefPtr > _body;
  Usul::Atomic::Integer < unsigned int > _level;
  Usul::Atomic::Object < Extents > _extents;
  Usul::Atomic::Object < Group::RefPtr > _root;
  Usul::Atomic::Object < Transform::RefPtr > _thisBranch;
  Usul::Atomic::Object < Group::RefPtr > _childBranch;
  Usul::Atomic::Object < Mesh::RefPtr > _mesh;
  Usul::Atomic::Container < TileVector > _children;
};


} // namespace Minerva


#endif // _MINERVA_DOCUMENT_TILE_CLASS_H_
