
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

#include "SceneGraph/Visitors/FrustumCull.h"
#include "SceneGraph/Common/ScopedStack.h"
#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/Nodes/Shapes/Line.h"
#include "SceneGraph/Nodes/Shapes/Geometry.h"

using namespace SceneGraph::Visitors;

///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef SceneGraph::Draw::Element DrawElement;
typedef SceneGraph::Nodes::Groups::Group Group;
typedef SceneGraph::Nodes::Groups::Transform Transform;
typedef SceneGraph::Nodes::Shapes::Line Line;
typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
typedef SceneGraph::Nodes::Shapes::Shape Shape;
typedef SceneGraph::Shaders::Shader Shader;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumCull::FrustumCull() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumCull::~FrustumCull()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add the shape.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template
  <
    class ShapeTypePtr, 
    class StateContainerTypePtr,
    class DrawListPtr,
    class MatrixType
  >
  inline void addShape ( 
    ShapeTypePtr &shape, 
    StateContainerTypePtr &stateContainer,
    DrawListPtr &dl,
    const MatrixType &matrix,
    int importance )
  {
    if ( ( true == shape.valid() ) && ( true == dl.valid() ) )
    {
      const int index ( shape->drawListIndexGet() );
      DrawElement::RefPtr de ( new DrawElement 
        ( shape, stateContainer, matrix, importance ) );
      dl->append ( index, de );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void FrustumCull::visit ( Line &n )
{
  Helper::addShape ( 
    Shape::RefPtr ( &n ), 
    n.stateContainer(),
    this->_drawListsGet(), 
    this->matrixStackTop(),
    n.importanceGet() );
  BaseClass::visit ( n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void FrustumCull::visit ( Geometry &n )
{
  Helper::addShape ( 
    Shape::RefPtr ( &n ), 
    n.stateContainer(),
    this->_drawListsGet(), 
    this->matrixStackTop(),
    n.importanceGet() );
  BaseClass::visit ( n );
}
