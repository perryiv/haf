
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Forward declarations of nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_FORWARD_CLASS_H_
#define _SCENE_GRAPH_FORWARD_CLASS_H_

#include "SceneGraph/Config/Config.h"


namespace SceneGraph
{
  namespace Nodes
  {
    namespace Groups
    {
      class Group;
      class Transform;
    }

    class Node;

    namespace Shapes
    {
      class Geometry;
      class Line;
      class Shape;
    }
  }

  namespace Shaders
  {
    class Shader;
  }

  namespace State
  {
    namespace Attributes
    {
      class BaseAttribute;
      class Blending;
      class Color;
      class CullFace;
      class Enable;
      class Hint;
      class Light;
      class LightModel;
      class LineWidth;
      class Material;
      class PolygonMode;
      class ShadeModel;
      class Texture2;
    }

    class Container;
  }

  namespace Visitors
  {
    class Visitor;
  }
}


#endif // _SCENE_GRAPH_FORWARD_CLASS_H_
