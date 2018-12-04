
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The draw method base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_DRAW_METHOD_CLASS_H_
#define _SCENE_GRAPH_DRAW_METHOD_CLASS_H_

#include "SceneGraph/Draw/Lists.h"
#include "SceneGraph/Visitors/Visitor.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector4.h"


namespace SceneGraph {
namespace Draw {


class SCENE_GRAPH_EXPORT Method : public SceneGraph::Visitors::Visitor
{
public:

  SCENE_GRAPH_OBJECT ( Method, SceneGraph::Visitors::Visitor );
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Math::Vec4d Viewport;
  typedef SceneGraph::Draw::Lists DrawLists;

  // Draw the elements in the list.
  virtual void            draw();

  // Set the draw-lists.
  void                    drawListsSet ( DrawLists::RefPtr );

  // Reset the visitor for use in the calling thread.
  virtual void            reset();

  // Get/set the navigation matrix.
  Matrix                  navigationMatrixGet() const;
  void                    navigationMatrixSet ( const Matrix & );

  // Get/set the projection matrix.
  Matrix                  projectionMatrixGet() const;
  void                    projectionMatrixSet ( const Matrix & );

  // Get/set the viewport.
  Viewport                viewportGet() const;
  void                    viewportSet ( const Viewport & );

protected:

  // Default construction.
  Method();

  // Use reference counting.
  virtual ~Method();

  DrawLists::RefPtr       _drawListsGet();

private:

  Usul::Atomic::Object < Matrix > _navigation;
  Usul::Atomic::Object < Matrix > _projection;
  Usul::Atomic::Object < Viewport > _viewport;
  Usul::Atomic::Object < DrawLists::RefPtr > _drawLists;
};


} // namespace Draw
} // namespace SceneGraph


#endif // _SCENE_GRAPH_DRAW_METHOD_CLASS_H_
