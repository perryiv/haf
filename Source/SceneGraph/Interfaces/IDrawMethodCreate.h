
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to create a draw-method.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_CREATE_DRAW_METHOD_H_
#define _SCENE_GRAPH_INTERFACES_CREATE_DRAW_METHOD_H_

#include "Usul/Interfaces/IUnknown.h"

#include "SceneGraph/Draw/Method.h"


namespace SceneGraph {
namespace Interfaces {


struct IDrawMethodCreate : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef SceneGraph::Draw::Method DrawMethod;
  typedef DrawMethod::RefPtr DrawMethodPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDrawMethodCreate );

  // Id for this interface.
  enum { IID = 1705069800u };

  // Pass a valid context.
  virtual DrawMethodPtr     createDrawMethod ( IUnknown::RefPtr context ) = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_CREATE_DRAW_METHOD_H_
