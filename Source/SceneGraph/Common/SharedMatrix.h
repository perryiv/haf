
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A reference counted matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHARED_MATRIX_CLASS_H_
#define _SCENE_GRAPH_SHARED_MATRIX_CLASS_H_

#include "SceneGraph/Base/Object.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Math/Matrix44.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix traits.
//
///////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{
  namespace Traits
  {
    template < class T > struct MatrixTraits;
    template <> struct MatrixTraits < Usul::Math::Matrix44d >
    {
      typedef Usul::Math::Matrix44d Matrix;
      static Matrix makeIdentity()
      {
        return Matrix::getIdentity();
      }
    };
    template <> struct MatrixTraits < Usul::Math::Matrix44f >
    {
      typedef Usul::Math::Matrix44f Matrix;
      static Matrix makeIdentity()
      {
        return Matrix::getIdentity();
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Shared matrix class.
//
///////////////////////////////////////////////////////////////////////////////

namespace SceneGraph {
namespace Common {


template < class T > class SharedMatrix : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( SharedMatrix, SceneGraph::Base::Object );
  typedef T Matrix;
  typedef typename SceneGraph::Traits::MatrixTraits<Matrix> MatrixTraits;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors.
  //
  /////////////////////////////////////////////////////////////////////////////

  SharedMatrix() : BaseClass(),
    _m ( MatrixTraits::makeIdentity() )
  {
  }
  explicit SharedMatrix ( const Matrix &m ) : BaseClass(),
    _m ( m )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const Matrix &m )
  {
    _m = m;
  }
  Matrix get() const
  {
    return _m;
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor. Use reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~SharedMatrix()
  {
  }

private:

  Usul::Atomic::Object < Matrix > _m;
};


} // namespace Common
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHARED_MATRIX_CLASS_H_
