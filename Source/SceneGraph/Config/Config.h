
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Conditional compile configurations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_CONFIG_H_
#define _SCENE_GRAPH_CONFIG_H_

#include "Usul/Config/Config.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the kind of container the group nodes use.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#include <list>
#define SCENE_GRAPH_GROUP_CONTAINER_TYPE \
  std::list < SceneGraph::Nodes::Node::RefPtr >
#endif

#if 1
#include <vector>
#define SCENE_GRAPH_GROUP_CONTAINER_TYPE \
  std::vector < SceneGraph::Nodes::Node::RefPtr >
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the kind of container each nodes uses for its parents.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#include <list>
#define SCENE_GRAPH_NODE_PARENT_CONTAINER_TYPE \
  std::list < SceneGraph::Nodes::Node * >
#endif

#if 1
#include <vector>
#define SCENE_GRAPH_NODE_PARENT_CONTAINER_TYPE \
  std::vector < SceneGraph::Nodes::Node * >
#endif


#endif // _SCENE_GRAPH_CONFIG_H_