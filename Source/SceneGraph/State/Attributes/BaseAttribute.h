
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all state attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_BASE_ATTRIBUTE_CLASS_H_
#define _SCENE_GRAPH_BASE_ATTRIBUTE_CLASS_H_

#include "SceneGraph/Visitors/Visitor.h"

#include "Usul/Errors/Assert.h"


namespace SceneGraph {
namespace State {
namespace Attributes {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all attributes.
//
///////////////////////////////////////////////////////////////////////////////

class SCENE_GRAPH_EXPORT BaseAttribute : public SceneGraph::Base::Object
{
public:
  SCENE_GRAPH_ATTRIBUTE ( BaseAttribute, SceneGraph::Base::Object );
  virtual void apply(){}
protected:
  BaseAttribute() : BaseClass(){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all exclusive attributes (e.g., PolygonMode or CullFace).
//
///////////////////////////////////////////////////////////////////////////////

class SCENE_GRAPH_EXPORT ExclusiveAttribute : public BaseAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( ExclusiveAttribute, BaseAttribute );
  virtual void apply(){}
protected:
  ExclusiveAttribute() : BaseClass(){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all non-exclusive attributes (e.g., LightModel or Enable).
//
///////////////////////////////////////////////////////////////////////////////

class SCENE_GRAPH_EXPORT NonExclusiveAttribute : public BaseAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( NonExclusiveAttribute, BaseAttribute );
  virtual void apply(){}
  virtual unsigned int key() const
  {
    USUL_ASSERT ( 0 ); // Need to overload this.
    return 0;
  }
protected:
  NonExclusiveAttribute() : BaseClass(){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Common enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Face
{
  enum Enum
  {
    FRONT = 0,
    BACK  = 1,
    BOTH  = 2
  };
}


} // namespace Attributes
} // namespace State
} // namespace SceneGraph


#endif // _SCENE_GRAPH_BASE_ATTRIBUTE_CLASS_H_
