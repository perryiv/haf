
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

#ifndef _MINERVA_DOCUMENT_BODY_CLASS_H_
#define _MINERVA_DOCUMENT_BODY_CLASS_H_

#include "Minerva/Interfaces/IBodyGet.h"
#include "Minerva/Document/Tile.h"

#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/Visitors/Visitor.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClearObject.h"
#include "Usul/Interfaces/IDestroyObject.h"
#include "Usul/Math/Ellipse.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include "boost/noncopyable.hpp"

#include <list>
#include <vector>


namespace Minerva {


class Body : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Minerva::Interfaces::IBodyGet,
  public Usul::Interfaces::IClearObject,
  public Usul::Interfaces::IDestroyObject
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef SceneGraph::Nodes::Node SceneNode;
  typedef SceneGraph::Nodes::Groups::Transform Transform;
  typedef SceneGraph::Visitors::Visitor SceneVisitor;
  typedef std::vector < Tile::RefPtr > TileVector;
  typedef std::list < Tile::RefPtr > TileList;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Ellipse < double > Ellipse;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef Tile::MeshSize MeshSize;
  typedef Tile::Extents Extents;

  USUL_REFERENCED_CLASS ( Body );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction
  Body();

  // Accept the scene visitor.
  void                            accept ( SceneVisitor & );

  // Minerva::Interfaces::IBodyGet
  const Body *                    bodyGet() const { return this; }
  Body *                          bodyGet()       { return this; }

  // Usul::Interfaces::IClearObject
  virtual void                    clearObject();

  // Conversion functions.
  Vec3d                           toXYZ ( const Vec3d &lonLatElev ) const;

  // Usul::Interfaces::IDestroyObject
  virtual void                    destroyObject();

  // Get the ellipse.
  Ellipse                         ellipse() const;

  // Get the default or appropriate mesh size for the extents.
  MeshSize                        meshSize() const { return _meshSize; }
  MeshSize                        meshSize ( const Extents &extents ) const;

  // Get the scene.
  Transform::RefPtr               scene();

protected:

  // Use reference counting.
  virtual ~Body();

  void                            _destroy();

private:

  Usul::Atomic::Object < Transform::RefPtr > _root;
  Usul::Atomic::Object < Ellipse > _ellipse;
  Usul::Atomic::Container < TileVector > _topTiles;
  Usul::Atomic::Container < TileList > _availableTiles;
  Usul::Atomic::Object < MeshSize > _meshSize;
};


} // namespace Minerva


#endif // _MINERVA_DOCUMENT_BODY_CLASS_H_
