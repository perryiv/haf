
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A quaternion class.
//
//  References:
//  http://www.cprogramming.com/tutorial/3d/quaternions.html
//  http://www.gamasutra.com/features/19980703/quaternions_01.htm
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEMPLATE_QUATERNION_CLASS_H_
#define _USUL_TEMPLATE_QUATERNION_CLASS_H_

#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Math {


template
<
  typename T,
  typename IndexType = unsigned int,
  typename ErrorCheckerType = Usul::Math::ErrorPolicy
>
class Quaternion
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Usul::Math::Vector3 < T, IndexType, ErrorCheckerType > VectorType;
  typedef typename VectorType::value_type value_type;
  typedef typename VectorType::size_type size_type;
  typedef typename VectorType::ErrorChecker ErrorChecker;
  typedef Quaternion < T, IndexType, ErrorCheckerType > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Quaternion() : _w ( 1 ), _v ( 0, 0, 0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Quaternion ( const Quaternion &q ) : _w ( q._w ), _v ( q._v )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Quaternion ( T w, const VectorType &v ) : _w ( w ), _v ( v )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Quaternion &q ) const
  {
    return ( ( _w == q._w ) && ( _v.equal ( q._v ) ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they "compare".
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Pred > bool compare ( const Quaternion &q, Pred p ) const
  {
    return ( p ( _w, q._w ) && p ( _v, q._v ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Execute the given predicate.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Pred > bool check ( Pred p ) const
  {
    return ( p ( _w ) && _v.check ( p ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the Cayley "norm".
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type normCayley() const
  {
    return ( ( _w * _w ) + ( _v.dot ( _v ) ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the magnitude.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type normEuclidian() const
  {
    return Usul::Math::sqrt ( this->normCayley() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Normalize this quaternion. Return the previous magnitude.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type normalize()
  {
    // Get the Cayley norm.
    const value_type n ( this->normCayley() );

    // Handle bad state.
    if ( 0 == n )
    {
      throw std::runtime_error ( "Error 1948353370: quaternion norm is zero" );
    }

    // See if already normalized.
    if ( 1 == n )
    {
      return n;
    }

    // Now calculate the magnitude.
    const value_type m ( Usul::Math::sqrt ( n ) );

    // Multiply by reciprocal for speed.
    const value_type invM ( static_cast < value_type > ( 1 ) / m );
    _w *= invM;
    _v *= invM;

    // Return original magnitude.
    return m;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a normalize quaternion.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType normalized() const
  {
    ThisType q ( *this );
    q.normalize();
    return q;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the "axis".
  //
  /////////////////////////////////////////////////////////////////////////////

  const VectorType &axis() const
  {
    return _v;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the "angle".
  //
  /////////////////////////////////////////////////////////////////////////////

  T angle() const
  {
    return _w;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the conjugate.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType conjugate() const
  {
    return ThisType ( _w, -_v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the product of the two quaternions.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void multiply ( const ThisType &q1, const ThisType &q2, ThisType &q )
  {
    const value_type w1 ( q1._w );
    const value_type x1 ( q1._v[0] );
    const value_type y1 ( q1._v[1] );
    const value_type z1 ( q1._v[2] );

    const value_type w2 ( q2._w );
    const value_type x2 ( q2._v[0] );
    const value_type y2 ( q2._v[1] );
    const value_type z2 ( q2._v[2] );

    const value_type angle ( w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2 );
    const VectorType axis ( w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
                            w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2,
                            w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2 );

    q = ThisType ( angle, axis );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the product of the two quaternions.
  //
  /////////////////////////////////////////////////////////////////////////////

  static ThisType multiply ( const ThisType &q1, const ThisType &q2 )
  {
    ThisType q;
    Quaternion::multiply ( q1, q2, q );
    return q;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return negative of this quaternion.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType operator - () const
  {
    return ThisType ( -_w, -_v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply by scalar.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType operator * ( value_type s ) const
  {
    return ThisType ( _w * s, _v * s );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply by scalar.
  //
  /////////////////////////////////////////////////////////////////////////////

  void operator *= ( value_type s )
  {
    _w *= s;
    _v *= s;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply by quaternion.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType operator * ( const ThisType &rhs ) const
  {
    return Quaternion::multiply ( *this, rhs );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply by quaternion.
  //
  /////////////////////////////////////////////////////////////////////////////

  void operator *= ( const ThisType &q )
  {
    Quaternion::multiply ( ThisType ( *this ), rhs, *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType operator + ( const ThisType &rhs ) const
  {
    return ThisType ( _w + rhs._w, _v + rhs._v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition.
  //
  /////////////////////////////////////////////////////////////////////////////

  void operator += ( const ThisType &rhs )
  {
    _w += rhs._w;
    _v += rhs._v;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the spherical linear interpolation.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void slerp ( const ThisType &from, const ThisType &to, value_type u, ThisType &q )
  {
    // Make a copy of the "to" quaternion.
    ThisType q2 ( to );

    // For readability below.
    const ThisType &q1 ( from );

    // To help with debugging.
    const value_type angle1 ( q1.angle() );
    const value_type angle2 ( q2.angle() );
    const VectorType &axis1 ( q1.axis() );
    const VectorType &axis2 ( q2.axis() );
    const value_type axisDot ( axis1.dot ( axis2 ) );

    // Cosine of angle between the two rotations is the 4D dot product.
    value_type cosAngle ( angle1 * angle2 + axisDot );

    // Adjust signs if needed to choose shortest arc.
    if ( cosAngle < 0 )
    { 
        cosAngle = -cosAngle; 
        q2 = -q2;
    }

    // These weights re-assigned below.
    value_type s1 ( 0 );
    value_type s2 ( 0 );

    // The dot product will be one -- or, because of roundoff error, slightly 
    // more than one -- if the two quaternions represent very "close" rotations.
    // Handle those cases by linearly interpolating because acos() returns 
    // NaN when the input is greater than one.
    const value_type one ( static_cast < value_type > ( 1 ) );
    if ( cosAngle >= one )
    {
      // Calculate the weights.
      s1 = one - u;
      s2 = u;
    }

    // The normal case.
    else
    {
      // Calculate the angle.
      const value_type angle ( Usul::Math::acos ( cosAngle ) );

      // The sine of the angle.
      const value_type sinAngle ( Usul::Math::sin ( angle ) );

      // Calculate the weights.
      s1 = Usul::Math::sin ( ( one - u ) * angle ) / sinAngle;
      s2 = Usul::Math::sin ( u * angle ) / sinAngle;
    }

    // Set the answer.
    q = ( q1 * s1 ) + ( q2 * s2 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the spherical linear interpolation.
  //
  /////////////////////////////////////////////////////////////////////////////

  static ThisType slerp ( const ThisType &from, const ThisType &to, value_type t )
  {
    ThisType q;
    Quaternion::slerp ( from, to, t, q );
    return q;
  }


private:

  T _w;
  VectorType _v;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Quaternion < float       > Quatf;
typedef Quaternion < double      > Quatd;
typedef Quaternion < long double > Quatld;


} // namespace Math
} // namespace Usul


#endif // _USUL_TEMPLATE_QUATERNION_CLASS_H_
