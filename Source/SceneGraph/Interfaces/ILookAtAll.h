
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to get a matrix that makes everything visible.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_LOOK_AT_ALL_MATRIX_H_
#define _SCENE_GRAPH_INTERFACES_LOOK_AT_ALL_MATRIX_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Matrix44.h"


namespace SceneGraph {
namespace Interfaces {


struct ILookAtAll : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Math::Matrix44d Matrix;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILookAtAll );

  // Id for this interface.
  enum { IID = 1120656404u };

  // Get the matrix.
  virtual void      lookAtAll ( Usul::Interfaces::IUnknown::RefPtr ) = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_LOOK_AT_ALL_MATRIX_H_
