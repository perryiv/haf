
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Matrix functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_MATRIX_FUNCTORS_H_
#define _SCENE_GRAPH_MATRIX_FUNCTORS_H_

#include "SceneGraph/Interfaces/ILookAtAll.h"
#include "SceneGraph/Interfaces/IMatrixGet.h"
#include "SceneGraph/Interfaces/IResizeNotify.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"


namespace SceneGraph {
namespace Functors {


///////////////////////////////////////////////////////////////////////////////
//
//  Base matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

class MatrixGet : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public SceneGraph::Interfaces::IMatrixGet
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef SceneGraph::Interfaces::IMatrixGet IMatrixGet;
  typedef IMatrixGet::Matrix Matrix;
  USUL_REFERENCED_CLASS ( MatrixGet );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MatrixGet();

  virtual Matrix matrixGet()
  {
    return Matrix::getIdentity();
  }

protected:

  virtual ~MatrixGet();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix functor that returns the given matrix.
//
///////////////////////////////////////////////////////////////////////////////

class MatrixReturn : 
  public MatrixGet,
  public SceneGraph::Interfaces::ILookAtAll
{
public:

  typedef MatrixGet BaseClass;
  typedef BaseClass::Matrix Matrix;
  typedef Usul::Interfaces::IUnknown IUnknown;
  USUL_REFERENCED_CLASS ( MatrixReturn );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MatrixReturn ( const Matrix &m ) : BaseClass(), _m ( m ){}

  virtual void lookAtAll ( IUnknown::RefPtr );

  virtual Matrix matrixGet()
  {
    return _m;
  }

protected:

  virtual ~MatrixReturn(){}

  void _setMatrix ( const Matrix &m )
  {
    _m = m;
  }

private:

  Usul::Atomic::Object < Matrix > _m;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix functor that calculates a perspective matrix.
//
///////////////////////////////////////////////////////////////////////////////

class Perspective : 
  public MatrixGet, 
  public SceneGraph::Interfaces::IResizeNotify
{
public:

  typedef MatrixGet BaseClass;
  USUL_REFERENCED_CLASS ( Perspective );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Perspective ( double fov, double aspect, double nearZ, double farZ ) : 
    BaseClass(), 
    _fov ( fov ),
    _aspect ( aspect ),
    _near ( nearZ ),
    _far ( farZ )
  {
  }

  virtual Matrix matrixGet()
  {
    return Matrix::perspective ( _fov, _aspect, _near, _far );
  }

  virtual void sizeSet ( double w, double h )
  {
    if ( ( w >= 0 ) && ( h >= 0 ) )
    {
      _aspect = ( w / h );
    }
  }

  double fov() const
  {
    return _fov;
  }

  double aspect() const
  {
    return _aspect;
  }

  double nearZ() const
  {
    return _near;
  }

  double farZ() const
  {
    return _far;
  }

  void fov ( double v )
  {
    _fov = v;
  }

  void aspect ( double v )
  {
    _aspect = v;
  }

  void nearZ ( double v )
  {
    _near = v;
  }

  void farZ ( double v )
  {
    _far = v;
  }

protected:

  virtual ~Perspective(){}

private:

  Usul::Atomic::Object < double > _fov;
  Usul::Atomic::Object < double > _aspect;
  Usul::Atomic::Object < double > _near;
  Usul::Atomic::Object < double > _far;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix functor that calculates the matrix using the given functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class MatrixCalculate : public MatrixGet
{
public:

  typedef MatrixGet BaseClass;
  USUL_REFERENCED_CLASS ( MatrixCalculate );

  MatrixCalculate ( Function f ) : BaseClass(), _function ( f ){}

  virtual Matrix matrixGet()
  {
    return _function();
  }

protected:

  virtual ~MatrixCalculate(){}

private:

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for making functors.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > 
inline SceneGraph::Interfaces::IMatrixGet::RefPtr makeMatrixFunctor ( F f )
{
  MatrixGet::RefPtr m ( new MatrixCalculate < F > ( f, p ) );
  SceneGraph::Interfaces::IMatrixGet::QueryPtr im ( m );
  return im;
}


} // namespace Functors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_MATRIX_FUNCTORS_H_
