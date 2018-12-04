
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The view-frustum cull visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_FRUSTUM_CULL_VISITOR_CLASS_H_
#define _SCENE_GRAPH_FRUSTUM_CULL_VISITOR_CLASS_H_

#include "SceneGraph/Visitors/CullVisitor.h"


namespace SceneGraph {
namespace Visitors {


class SCENE_GRAPH_EXPORT FrustumCull : public CullVisitor
{
public:

  SCENE_GRAPH_OBJECT ( FrustumCull, CullVisitor );

  // Default construction.
  FrustumCull();

  // Visit nodes.
  virtual void            visit ( SceneGraph::Nodes::Shapes::Line & );
  virtual void            visit ( SceneGraph::Nodes::Shapes::Geometry & );

protected:

  // Use reference counting.
  virtual ~FrustumCull();
};


} // namespace Visitors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_FRUSTUM_CULL_VISITOR_CLASS_H_
