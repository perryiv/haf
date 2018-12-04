
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Trackball navigation matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_TRACKBALL_MATRIX_FUNCTOR_H_
#define _SCENE_GRAPH_TRACKBALL_MATRIX_FUNCTOR_H_

#include "SceneGraph/Functors/Matrix.h"

#include "Usul/Math/Vector2.h"


namespace SceneGraph {
namespace Functors {


class Trackball : public MatrixReturn
{
public:

  typedef MatrixReturn BaseClass;
  typedef BaseClass::Matrix Matrix;
  typedef Usul::Math::Vec2d MousePoint;
  USUL_REFERENCED_CLASS ( Trackball );

  Trackball() : BaseClass ( Matrix::getIdentity() ),
    _m0 ( 0, 0 ), _m1 ( 0, 0 )
  {
  }

  void mouseInitialSet ( const MousePoint &m )
  {
    _m0 = m;
  }

  void mouseFinalSet ( const MousePoint &m )
  {
    _m1 = m;
  }

  MousePoint mouseFinalGet() const
  {
    return _m1;
  }

protected:

  virtual ~Trackball()
  {
  }

private:

  MousePoint _m0;
  MousePoint _m1;
};


} // namespace Functors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_TRACKBALL_MATRIX_FUNCTOR_H_
