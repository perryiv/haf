
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The cull visitor base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_CULL_VISITOR_CLASS_H_
#define _SCENE_GRAPH_CULL_VISITOR_CLASS_H_

#include "SceneGraph/Visitors/Visitor.h"
#include "SceneGraph/Draw/Lists.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector4.h"


namespace SceneGraph {
namespace Visitors {


class SCENE_GRAPH_EXPORT CullVisitor : public SceneGraph::Visitors::Visitor
{
public:

  SCENE_GRAPH_OBJECT ( CullVisitor, SceneGraph::Visitors::Visitor );
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Math::Vec4d Viewport;
  typedef SceneGraph::Draw::Lists DrawLists;

  // Set the draw-lists.
  void                    drawListsSet ( DrawLists::RefPtr );

  // Reset the visitor for use in the calling thread.
  virtual void            reset();

  // Set the navigation matrix.
  void                    navigationMatrixSet ( const Matrix & );

  // Set the projection matrix.
  void                    projectionMatrixSet ( const Matrix & );

  // Get/set the viewport.
  Viewport                viewportGet() const;
  void                    viewportSet ( const Viewport & );

protected:

  // Default construction.
  CullVisitor();

  // Use reference counting.
  virtual ~CullVisitor();

  DrawLists::RefPtr       _drawListsGet();

private:

  Usul::Atomic::Object < Matrix > _navigation;
  Usul::Atomic::Object < Matrix > _projection;
  Usul::Atomic::Object < Viewport > _viewport;
  Usul::Atomic::Object < DrawLists::RefPtr > _drawLists;
};


} // namespace Visitors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_CULL_VISITOR_CLASS_H_
