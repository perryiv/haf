
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The update visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_UPDATE_VISITOR_CLASS_H_
#define _SCENE_GRAPH_UPDATE_VISITOR_CLASS_H_

#include "SceneGraph/Visitors/Visitor.h"


namespace SceneGraph {
namespace Visitors {


class SCENE_GRAPH_EXPORT UpdateVisitor : public SceneGraph::Visitors::Visitor
{
public:

  SCENE_GRAPH_OBJECT ( UpdateVisitor, SceneGraph::Visitors::Visitor );

  // Default construction.
  UpdateVisitor();

protected:

  // Use reference counting.
  virtual ~UpdateVisitor();
};


} // namespace Visitors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_UPDATE_VISITOR_CLASS_H_
