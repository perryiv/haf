
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Conversions between 4x4 matrices and quaternions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONVERT_MATRIX_QUATERNION_CONVERSIONS_H_
#define _USUL_CONVERT_MATRIX_QUATERNION_CONVERSIONS_H_

#include "Usul/Config/Config.h"
#include "Usul/Convert/Generic.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Quaternion.h"
#include "Usul/Math/Vector4.h"
#include "Usul/MPL/SameType.h"


namespace Usul { namespace Convert {


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from quaternion to matrix.
//  See: http://www.gamasutra.com/features/19980703/quaternions_01.htm
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class QuatType, class MatrixType > 
  inline void quatToMatrix ( const QuatType &q, MatrixType &m )
  {
    typedef typename MatrixType::value_type ValueType;
    USUL_ASSERT_SAME_TYPE ( typename QuatType::value_type, ValueType );
    typedef typename QuatType::VectorType VectorType;

    const QuatType::VectorType axis ( q.axis() );
    const ValueType w ( q.angle() );
    const ValueType x ( axis[0] );
    const ValueType y ( axis[1] );
    const ValueType z ( axis[2] );

    const ValueType x2 ( x + x );
    const ValueType y2 ( y + y );
    const ValueType z2 ( z + z );

    const ValueType xx ( x * x2 );
    const ValueType xy ( x * y2 );
    const ValueType xz ( x * z2 );

    const ValueType yy ( y * y2 );
    const ValueType yz ( y * z2 );
    const ValueType zz ( z * z2 );

    const ValueType wx ( w * x2 );
    const ValueType wy ( w * y2 );
    const ValueType wz ( w * z2 );

    const ValueType one ( 1 );
    m.set ( 
      one - ( yy + zz ),          xy + wz,            xz - wy,    0,
              xy - wz,    one - ( xx + zz ),          yz + wx,    0,
              xz + wy,            yz - wx,    one - ( xx + yy ),  0,
                    0,                  0,                    0,  1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from matrix to quaternion.
//  Implementation same as osg::Matrix "David Spillings implementation Mk 1".
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class QuatType, class MatrixType > 
  inline void matrixToQuat ( const MatrixType &m, QuatType &q )
  {
    typedef typename QuatType::value_type ValueType;
    USUL_ASSERT_SAME_TYPE ( typename MatrixType::value_type, ValueType );
    typedef typename QuatType::VectorType VectorType;

    // Initialize.
    ValueType w ( 0 ), x ( 0 ), y ( 0 ), z ( 0 );

    // Use tq to store the largest trace.
    const ValueType one ( static_cast < ValueType > ( 1 ) );
    typedef Usul::Math::Vector4 < ValueType > Vec4;
    Vec4 tq ( ( one + m(0,0) + m(1,1) + m(2,2) ),
              ( one + m(0,0) - m(1,1) - m(2,2) ),
              ( one - m(0,0) + m(1,1) - m(2,2) ),
              ( one - m(0,0) - m(1,1) + m(2,2) ) );

    // Find the maximum.
    unsigned int j ( 0 );
    for ( unsigned int i = 1; i < 4; ++i )
    {
      j = ( ( tq[i] > tq[j] ) ? i : j );
    }

    // Check the diagonal.
    switch ( j )
    {
    case 0:

      w = tq[0];
      x = m(1,2) - m(2,1);
      y = m(2,0) - m(0,2);
      z = m(0,1) - m(1,0);
      break;

    case 1:

      w = m(1,2) - m(2,1);
      x = tq[1];
      y = m(0,1) + m(1,0);
      z = m(2,0) + m(0,2);
      break;

    case 2:

      w = m(2,0) - m(0,2);
      x = m(0,1) + m(1,0);
      y = tq[2];
      z = m(1,2) + m(2,1);
      break;

    default:

      w = m(0,1) - m(1,0);
      x = m(2,0) + m(0,2);
      y = m(1,2) + m(2,1);
      z = tq[3];
      break;
    }

    // Calculate scales.
    const ValueType quarter ( static_cast < ValueType > ( 0.25 ) );
    const ValueType s ( Usul::Math::sqrt < ValueType > ( quarter / tq[j] ) );
    w *= s;
    x *= s;
    y *= s;
    z *= s;

    // Assign answer
    q = QuatType ( w, VectorType ( x, y, z ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert between quaternion and matrix.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CONVERT_DEFINE_MATRIX_QUATERNION_CONVERTER(quat_type,matrix_type)\
template <> struct Type < quat_type, matrix_type >\
{\
  typedef quat_type FromType;\
  typedef matrix_type ToType;\
  typedef Type < FromType, ToType > ThisType;\
  static void convert ( const FromType &from, ToType &to )\
  {\
    Helper::quatToMatrix ( from, to );\
  }\
  static ToType convert ( const FromType &from )\
  {\
    ToType to;\
    ThisType::convert ( from, to );\
    return to;\
  }\
};\
template <> struct Type < matrix_type, quat_type >\
{\
  typedef matrix_type FromType;\
  typedef quat_type ToType;\
  typedef Type < FromType, ToType > ThisType;\
  static void convert ( const FromType &from, ToType &to )\
  {\
    Helper::matrixToQuat ( from, to );\
  }\
  static ToType convert ( const FromType &from )\
  {\
    ToType to;\
    ThisType::convert ( from, to );\
    return to;\
  }\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare some converters.
//
///////////////////////////////////////////////////////////////////////////////

USUL_CONVERT_DEFINE_MATRIX_QUATERNION_CONVERTER ( Usul::Math::Quatf,  Usul::Math::Matrix44f );
USUL_CONVERT_DEFINE_MATRIX_QUATERNION_CONVERTER ( Usul::Math::Quatd,  Usul::Math::Matrix44d );
USUL_CONVERT_DEFINE_MATRIX_QUATERNION_CONVERTER ( Usul::Math::Quatld, Usul::Math::Matrix44ld );


} // namespace Convert
} // namespace Usul


#endif // _USUL_CONVERT_MATRIX_QUATERNION_CONVERSIONS_H_
