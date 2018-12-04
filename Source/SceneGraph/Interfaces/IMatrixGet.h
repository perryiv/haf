
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to get a matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_GET_MATRIX_H_
#define _SCENE_GRAPH_INTERFACES_GET_MATRIX_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Matrix44.h"


namespace SceneGraph {
namespace Interfaces {


struct IMatrixGet : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Math::Matrix44d Matrix;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMatrixGet );

  // Id for this interface.
  enum { IID = 3498907089u };

  // Get the matrix.
  virtual Matrix        matrixGet() = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_GET_MATRIX_H_
