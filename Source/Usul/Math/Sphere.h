
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A sphere class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_SPHERE_CLASS_H_
#define _USUL_MATH_SPHERE_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Math {


template < typename T > class Sphere
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef T value_type;
  typedef T ValueType;
  typedef Usul::Math::Vector3 < T > Vector;
  typedef Sphere < T > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Sphere() : _c ( 0, 0, 0 ), _r ( 1 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Sphere ( const Vector &center, ValueType radius ) : _c ( center ), _r ( radius )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Sphere ( const ThisType &s ) : _c ( s._c ), _r ( s._r )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Sphere &operator = ( const ThisType &s )
  {
    _c = s._c;
    _r = s._r;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the radius.
  //
  /////////////////////////////////////////////////////////////////////////////

  void radius ( const ValueType &r )
  {
    _r = r;
  }
  ValueType radius() const
  {
    return _r;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the center.
  //
  /////////////////////////////////////////////////////////////////////////////

  void center ( const Vector &c )
  {
    _c = c;
  }
  const Vector &center() const
  {
    return _c;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Sphere &s ) const
  {
    return ( ( _c.equal ( s._c ) ) && ( _r == s._r ) );
  }
  bool operator == ( const Sphere &s ) const
  {
    return this->equal ( s );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a sphere that bounds the given geometry.
  //
  /////////////////////////////////////////////////////////////////////////////

  static Sphere bounds ( const Sphere &s1, const Sphere &s2 )
  {
    // Get the centers and radii for convenience.
    const Vector &c1 ( s1.center() );
    const Vector &c2 ( s2.center() );
    const ValueType r1 ( s1.radius() );
    const ValueType r2 ( s2.radius() );

    // Make a vector from c1 to c2.
    Vector c1c2 ( c2 - c1 );

    // Handle coincident centers.
    if ( ( 0 == c1c2[0] ) && ( 0 == c1c2[1] ) && ( 0 == c1c2[2] ) )
    {
      // Make sphere with center and maximum radius.
      const ValueType maxRadius ( Usul::Math::maximum ( r1, r2 ) );
      return Sphere ( c1, maxRadius );
    }

    // Handle s1 completely inside s2.
    const ValueType dist ( c1.distance ( c2 ) );
    if ( ( dist + r1 ) < r2 )
    {
      return s2;
    }

    // Handle s2 completely inside s1.
    if ( ( dist + r2 ) < r1 )
    {
      return s1;
    }

    // Normalize this vector.
    c1c2.normalize();

    // Extend rays along c1c2 to find extreme point on s1 and s2.
    const Vector p1 ( c1 - c1c2 * r1 );
    const Vector p2 ( c2 + c1c2 * r2 );

    // The new center is the midpoint.
    const ValueType half ( static_cast < ValueType > ( 0.5 ) );
    const Vector c ( ( p1 + p2 ) * half );

    // The new radius is half the distance between them.
    const ValueType r ( p1.distance ( p2 ) * half );

    // Return the new sphere.
    return Sphere ( c, r );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a sphere that bounds the given point(s).
  //  Note: a point is the same as a sphere with zero radius.
  //
  /////////////////////////////////////////////////////////////////////////////

  static Sphere bounds ( const Sphere &s, const Vector &pt )
  {
    return Sphere::bounds ( s, Sphere ( pt, 0 ) );
  }
  static Sphere bounds ( const Vector &pt, const Sphere &s )
  {
    return Sphere::bounds ( Sphere ( pt, 0 ), s );
  }
  static Sphere bounds ( const Vector &pt1, const Vector &pt2 )
  {
    return Sphere::bounds ( Sphere ( pt1, 0 ), Sphere ( pt2, 0 ) );
  }
  template < class PointType > 
  static Sphere bounds ( const Sphere &s, const PointType &pt )
  {
    return Sphere::bounds ( s, Vector ( pt[0], pt[1], pt[2] ) );
  }
  template < class PointType > 
  static Sphere bounds ( const PointType &pt, const Sphere &s )
  {
    return Sphere::bounds ( Vector ( pt[0], pt[1], pt[2] ), s );
  }
  template < class PointType > 
  static Sphere bounds ( const PointType &pt1, const PointType &pt2 )
  {
    return Sphere::bounds ( Vector ( pt1[0], pt1[1], pt1[2] ), 
                            Vector ( pt2[0], pt2[1], pt2[2] ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Intersect this sphere with the given line. Returns the number of hits.
  //  If it hits then u1 will hold the parameteric value along the line.
  //
  //  The point can be found with:
  //
  //    intersected_point_1 = pt1 + u1 * ( pt2 - pt1 )
  //
  //  If there is a second hit then the same applies:
  //
  //    intersected_point_2 = pt2 + u2 * ( pt2 - pt1 )
  //
  //  See: http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int intersect ( const Vector &pt1, const Vector &pt2, 
                           ValueType &u1, ValueType &u2, 
                           ValueType tol = 1e-6 ) const
  {
    const ValueType x1 ( pt1[0] );
    const ValueType y1 ( pt1[1] );
    const ValueType z1 ( pt1[2] );

    const ValueType x2 ( pt2[0] );
    const ValueType y2 ( pt2[1] );
    const ValueType z2 ( pt2[2] );

    const ValueType x3 ( _c[0] );
    const ValueType y3 ( _c[1] );
    const ValueType z3 ( _c[2] );

    const ValueType a ( ( ( x2 - x1 ) * ( x2 - x1 ) ) +
                        ( ( y2 - y1 ) * ( y2 - y1 ) ) +
                        ( ( z2 - z1 ) * ( z2 - z1 ) ) );

    const ValueType b ( ( ( ( x2 - x1 ) * ( x1 - x3 ) ) +
                          ( ( y2 - y1 ) * ( y1 - y3 ) ) +
                          ( ( z2 - z1 ) * ( z1 - z3 ) ) ) * 2 );

    const ValueType c ( ( x3 * x3 ) + ( y3 * y3 ) + ( z3 * z3 ) +
                        ( x1 * x1 ) + ( y1 * y1 ) + ( z1 * z1 ) -
                        ( ( ( x3 * x1 ) + ( y3 * y1 ) + ( z3 * z1 ) ) * 2 ) -
                        ( _r * _r ) );

    // a^2 * u + b^2 * u + c = 0
    const ValueType inner ( ( b * b ) - ( a * c * 4 ) );

    // Inner expression equals zero?
    if ( ( inner < tol ) && ( inner > -tol ) )
    {
      u1 = -b / ( a * 2 );
      return 1;
    }
    else if ( inner < 0 )
    {
      return 0;
    }
    else
    {
      u1 = ( -b + Usul::Math::sqrt ( inner ) ) / ( a * 2 );
      u2 = ( -b - Usul::Math::sqrt ( inner ) ) / ( a * 2 );
      return 2;
    }
  }
  template < class PointType > 
  void intersect ( const PointType &pt1, const PointType &pt2, 
                   ValueType &u1, ValueType &u2, 
                   ValueType tol = 1e-6 ) const
  {
    return Sphere::intersect ( Vector ( pt1[0], pt1[1], pt1[2] ), 
                               Vector ( pt2[0], pt2[1], pt2[2] ),
                               u1, u2, tol );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return min and max of the box that contains sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector boxMin() const
  {
    return Vector ( _c[0] - _r, _c[1] - _r, _c[2] - _r );
  }
  Vector boxMax() const
  {
    return Vector ( _c[0] + _r, _c[1] + _r, _c[2] + _r );
  }

private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Internal data.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType _r;
  Vector _c;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Sphere < float       > Spheref;
typedef Sphere < double      > Sphered;
typedef Sphere < long double > Sphereld;


} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_SPHERE_CLASS_H_
