
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Enumerations for nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_COMMON_ENUMERATIONS_CLASS_H_
#define _SCENE_GRAPH_COMMON_ENUMERATIONS_CLASS_H_

#include "SceneGraph/Config/Config.h"


namespace SceneGraph {
namespace Common {


// Modes of visiting children.
struct VisitMode
{
  enum Mode
  {
    LOCK_CHILDREN, // Lock container of children then visit them.
    COPY_CHILDREN  // Lock, copy child smare-pointers, visit them.
  };
};


} // namespace Common
} // namespace SceneGraph


#endif // _SCENE_GRAPH_COMMON_ENUMERATIONS_CLASS_H_
