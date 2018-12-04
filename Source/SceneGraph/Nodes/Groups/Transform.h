
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The transform group.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_TRANSFORM_GROUP_CLASS_H_
#define _SCENE_GRAPH_TRANSFORM_GROUP_CLASS_H_

#include "SceneGraph/Nodes/Groups/Group.h"
#include "SceneGraph/Common/SharedMatrix.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Math/Matrix44.h"


namespace SceneGraph {
namespace Nodes {
namespace Groups {


class SCENE_GRAPH_EXPORT Transform : public Group
{
public:

  SCENE_GRAPH_NODE ( Transform, Group );

  // Typedefs.
  typedef Usul::Math::Matrix44d Matrix;
  typedef SceneGraph::Common::SharedMatrix<Matrix> SharedMatrix;

  // Construction
  Transform();
  Transform ( const Matrix & );
  Transform ( SharedMatrix::RefPtr );

  // Get/set the matrix.
  Matrix                        matrixGet() const;
  void                          matrixSet ( const Matrix & );

  // Get/set the shared matrix.
  SharedMatrix::RefPtr          sharedMatrixGet();
  void                          sharedMatrixSet ( SharedMatrix::RefPtr );

protected:

  // Use reference counting.
  virtual ~Transform();

  virtual void                  _updateBoundingBox();
  virtual void                  _updateBoundingSphere();

private:

  void                          _destroy();

  Usul::Atomic::Object < SharedMatrix::RefPtr > _matrix;
};


} // namespace Groups
} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_TRANSFORM_GROUP_CLASS_H_
