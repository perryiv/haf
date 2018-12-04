
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Visitors/Visitor.h"
#include "SceneGraph/Common/ScopedStack.h"
#include "SceneGraph/Nodes/Shapes/Line.h"
#include "SceneGraph/Nodes/Shapes/Geometry.h"
#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/State/Container.h"

#include "Usul/Functions/NoThrow.h"

using namespace SceneGraph::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Visitor ( VisitMode::Mode gvm, VisitMode::Mode avm ) : BaseClass(),
  _groupVisitMode ( gvm ),
  _attributeVisitMode ( avm ),
  _nodePath(),
  _matrixStack()
{
  _matrixStack.push_back ( Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::~Visitor()
{
  USUL_TRY_BLOCK
  {
    _nodePath.clear();
    _matrixStack.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1320670679" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the visit-mode.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::VisitMode::Mode Visitor::attributeVisitMode() const
{
  // This member is const.
  return _attributeVisitMode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the visit-mode.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::VisitMode::Mode Visitor::groupVisitMode() const
{
  // This member is const.
  return _groupVisitMode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::reset()
{
  _nodePath.clear();
  _matrixStack.clear();
  _matrixStack.push_back ( Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the top of the matrix stack.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Matrix Visitor::matrixStackTop() const
{
  AtomicMatrixStack::Guard ( _matrixStack.mutex() );
  const MatrixStack &ms ( _matrixStack.getReference() );
  return ( ( false == ms.empty() ) ? ms.back() : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Groups::Group &g )
{
  typedef SceneGraph::Nodes::Groups::Group Group;

  // Push and pop the node from the path.
  typedef Common::ScopedStack<AtomicNodePath> ScopedNodePath;
  ScopedNodePath scopedNodePath ( _nodePath, ObjectPtr ( &g ) );

  // Redirect to the group.
  g.accept ( *this, _groupVisitMode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the state-container.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::State::Container &c )
{
  c.accept ( *this, _attributeVisitMode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Shapes::Line &n )
{
  typedef SceneGraph::Nodes::Shapes::Line Line;
  this->visit ( static_cast < Line::BaseClass & > ( n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Shapes::Geometry &n )
{
  typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
  this->visit ( static_cast < Geometry::BaseClass & > ( n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Node & )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Shapes::Shape &n )
{
  typedef SceneGraph::Nodes::Shapes::Shape Shape;
  this->visit ( static_cast < Shape::BaseClass & > ( n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( SceneGraph::Nodes::Groups::Transform &t )
{
  typedef SceneGraph::Nodes::Groups::Transform Transform;

  // Push and pop the accumulated matrix.
  const Matrix m ( _matrixStack.back() * t.matrixGet() );
  Common::ScopedStack<AtomicMatrixStack> pushPop ( _matrixStack, m );

  // Visit the base class.
  this->visit ( static_cast < Transform::BaseClass & > ( t ) );
}
