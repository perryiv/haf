
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all shape nodes.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Nodes/Shapes/Shape.h"

#include "Usul/Functions/NoThrow.h"

using namespace SceneGraph::Nodes::Shapes;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Shape::Shape() : BaseClass(),
  _drawListIndex ( 0 ),
  _importance ( 0 ),
  _stateContainer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Shape::~Shape()
{
  USUL_TRY_BLOCK
  {
    // Done with these.
    _stateContainer = StateContainer::RefPtr();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3155912481" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw-list index.
//
///////////////////////////////////////////////////////////////////////////////

void Shape::drawListIndexSet ( int index )
{
  _drawListIndex = index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw-list index.
//
///////////////////////////////////////////////////////////////////////////////

int Shape::drawListIndexGet() const
{
  return _drawListIndex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the importance.
//
///////////////////////////////////////////////////////////////////////////////

void Shape::importanceSet ( int importance )
{
  _importance = importance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the importance.
//
///////////////////////////////////////////////////////////////////////////////

int Shape::importanceGet() const
{
  return _importance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state-container.
//
///////////////////////////////////////////////////////////////////////////////

void Shape::stateContainer ( StateContainer::RefPtr sc )
{
  _stateContainer = sc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state-container.
//
///////////////////////////////////////////////////////////////////////////////

Shape::StateContainer::RefPtr Shape::stateContainer ( bool createIfNeeded )
{
  Guard guard ( _stateContainer.mutex() );
  StateContainer::RefPtr &ss ( _stateContainer.getReference() );
  if ( false == ss.valid() )
  {
    if ( true == createIfNeeded )
    {
      ss = StateContainer::RefPtr ( new StateContainer );
    }
  }
  return ss;
}
