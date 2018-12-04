
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

#ifndef _SCENE_GRAPH_NODE_CLASS_H_
#define _SCENE_GRAPH_NODE_CLASS_H_

#include "SceneGraph/Visitors/Visitor.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Math/Box.h"
#include "Usul/Math/Sphere.h"


namespace SceneGraph {
namespace Nodes {


class SCENE_GRAPH_EXPORT Node : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_NODE ( Node, SceneGraph::Base::Object );

  typedef std::pair < bool, Usul::Math::Sphered > BoundingSphere;
  typedef Usul::Math::Boxd BoundingBox;
  typedef SCENE_GRAPH_NODE_PARENT_CONTAINER_TYPE Parents;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;

  // Possible flags.
  enum
  {
    DIRTY_BOUNDS         = 0x00000001,
    CONTRIBUTE_TO_BOUNDS = 0x00000002
  };

  // Get/set the bounding box.
  BoundingBox                   boundingBoxGet() const;
  void                          boundingBoxSet ( const BoundingBox & );

  // Get/set the bounding sphere.
  BoundingSphere                boundingSphereGet() const;
  void                          boundingSphereSet ( const BoundingSphere & );

  // Should this node contribute to the bounds?
  bool                          contributeToBoundsGet() const;
  void                          contributeToBoundsSet ( bool );

  // Set the dirty flags.
  void                          dirtyBounds ( bool state, bool notifyParents = true );

  // Get the flags.
  bool                          isDirtyBounds() const;

  // Add/remove a parent. Normal use does not require calling this.
  void                          parentAdd ( Node * );
  void                          parentRemove ( Node * );

protected:

  // Default construction.
  Node();

  // Use reference counting.
  virtual ~Node();

  unsigned int                  _flagsGet() const;
  bool                          _flagsGet ( unsigned int flags ) const;
  void                          _flagsSet ( unsigned int flags );
  void                          _flagsSet ( unsigned int flags, bool state );

  virtual void                  _updateBoundingBox(){}
  virtual void                  _updateBoundingSphere(){}

  Usul::Atomic::Integer < unsigned int > _flags;
  Usul::Atomic::Object < BoundingSphere > _bSphere;
  Usul::Atomic::Object < BoundingBox > _bBox;
  Usul::Atomic::Container < Parents > _parents;
};


} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_NODE_CLASS_H_
