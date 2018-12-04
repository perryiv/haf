
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test functions for Usul's math classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Math/Constants.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Sphere.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Predicates/CloseFloat.h"

#include "boost/test/unit_test.hpp"


namespace Tests {
namespace Usul {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Details
{
  template < class T > inline void test001()
  {
    typedef ::Usul::Math::Vector2<T> Vec2;
    typedef ::Usul::Math::Vector3<T> Vec3;
    typedef ::Usul::Math::Vector4<T> Vec4;
    typedef ::Usul::Math::Matrix44<T> Matrix;
    {
      const Vec2 a ( 1, 2 );
      const Vec2 b ( 2, 3 );
      BOOST_CHECK_EQUAL ( static_cast < T > ( 8 ), a.dot ( b ) );
    }
    {
      const Vec3 a ( 1, 2, 3 );
      const Vec3 b ( 2, 3, 4 );
      BOOST_CHECK_EQUAL ( static_cast < T > ( 20 ), a.dot ( b ) );
    }
    {
      const Vec4 a ( 1, 2, 3, 4 );
      const Vec4 b ( 2, 3, 4, 5 );
      BOOST_CHECK_EQUAL ( static_cast < T > ( 40 ), a.dot ( b ) );
    }
    {
      const Vec4 a ( 3, 4, 5, 6 );
      const Vec4 b ( 50, 63, 101, 99 );
      const Matrix m ( 1, 2, 3, 4, 2, 3, 3, 5, 3, 4, 8, 6, 4, 5, 5, 7 );
      BOOST_CHECK ( b.equal ( m * a ) );

      const Matrix tm ( 1, 2, 3, 4, 2, 3, 4, 5, 3, 3, 8, 5, 4, 5, 6, 7 );
      BOOST_CHECK ( tm.equal ( m.transposed() ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Details
{
  template < class T > inline void test002()
  {
    typedef ::Usul::Math::Vector2<T> Vec2;
    typedef ::Usul::Math::Vector3<T> Vec3;
    typedef ::Usul::Math::Vector4<T> Vec4;
    typedef ::Usul::Math::Matrix44<T> Matrix;

    const Matrix t ( Matrix::translation ( 100, 10, 1 ) );
    const T angle ( static_cast < T > ( ::Usul::Math::DEG_TO_RAD ) * 45 );
    const Vec3 axis ( Vec3 ( 1, 2, 3 ).normalized() );
    const Matrix r ( Matrix::rotation ( angle, axis ) );
    const Matrix m1 (  0.728027725387508, -0.525104821111919,  0.440727305612110,  67.992451633243775,
                       0.608788597915763,  0.790790557990391, -0.063456571298848,  68.723308800181343,
                      -0.315201640406345,  0.314507901710379,  0.895395278995196, -27.479689744535467,
                       0.000000000000000,  0.000000000000000,  0.000000000000000,   1.000000000000000 );
    const Matrix m2 (  0.728027725387508, -0.525104821111919,  0.440727305612110, 100.000000000000000,
                       0.608788597915763,  0.790790557990391, -0.063456571298848,  10.000000000000000,
                      -0.315201640406345,  0.314507901710379,  0.895395278995196,   1.000000000000000,
                       0.000000000000000,  0.000000000000000,  0.000000000000000,   1.000000000000000 );

    // See if the numbers are within this many adjacent floating point values.
    ::Usul::Predicates::CloseFloat < T > pred ( 19 );

    BOOST_CHECK ( true == pred ( static_cast < T > ( 1 ), t.determinant() ) );
    BOOST_CHECK ( true == pred ( static_cast < T > ( 1 ), r.determinant() ) );
    BOOST_CHECK ( true == pred ( static_cast < T > ( 1 ), m1.determinant() ) );
    BOOST_CHECK ( true == pred ( static_cast < T > ( 1 ), m2.determinant() ) );
    BOOST_CHECK ( m1.compare ( r * t, pred ) );
    BOOST_CHECK ( m2.compare ( t * r, pred ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Details
{
  template < class T > inline void test003()
  {
    typedef ::Usul::Math::Vector3<T> Vec3;
    typedef ::Usul::Math::Vector4<T> Vec4;
    typedef ::Usul::Math::Matrix44<T> Matrix;

    const T x ( 10 );
    const Matrix t ( Matrix::translation ( x, 0, 0 ) );
    const Vec3 axis ( 0, 1, 0 );
    const Matrix r ( Matrix::rotation<90> ( axis ) );
    const Matrix m1 ( t * r );
    const Matrix m2 ( r * t );

    // See if the numbers are within this many adjacent floating point values.
    ::Usul::Predicates::CloseFloat < T > pred ( 1 );

    BOOST_CHECK ( Vec4 ( x, 0,  0, 1 ).compare (  t * Vec4 ( 0, 0, 0, 1 ), pred ) );
    BOOST_CHECK ( Vec4 ( 0, 0, -x, 1 ).compare (  r * Vec4 ( x, 0, 0, 1 ), pred ) );
    BOOST_CHECK ( Vec4 ( x, 0,  0, 1 ).compare ( m1 * Vec4 ( 0, 0, 0, 1 ), pred ) );
    BOOST_CHECK ( Vec4 ( x, 0, -x, 1 ).compare ( m1 * Vec4 ( x, 0, 0, 1 ), pred ) );
    BOOST_CHECK ( Vec4 ( 0, 0, -x, 1 ).compare ( m2 * Vec4 ( 0, 0, 0, 1 ), pred ) );
    BOOST_CHECK ( Vec4 ( 0, x, -x, 1 ).compare ( m2 * Vec4 ( 0, x, 0, 1 ), pred ) );

    BOOST_CHECK ( Vec3 ( x, 0,  0 ).compare (  t * Vec3 ( 0, 0, 0 ), pred ) );
    BOOST_CHECK ( Vec3 ( 0, 0, -x ).compare (  r * Vec3 ( x, 0, 0 ), pred ) );
    BOOST_CHECK ( Vec3 ( x, 0,  0 ).compare ( m1 * Vec3 ( 0, 0, 0 ), pred ) );
    BOOST_CHECK ( Vec3 ( x, 0, -x ).compare ( m1 * Vec3 ( x, 0, 0 ), pred ) );
    BOOST_CHECK ( Vec3 ( 0, 0, -x ).compare ( m2 * Vec3 ( 0, 0, 0 ), pred ) );
    BOOST_CHECK ( Vec3 ( 0, x, -x ).compare ( m2 * Vec3 ( 0, x, 0 ), pred ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Details
{
  template < class T > inline void test004()
  {
    typedef ::Usul::Math::Sphere<T> Sphere;
    typedef Sphere::Vector Vec;

    // Identical spheres should be equal.
    {
      const Sphere a ( Vec ( 10, 0, 0 ), 1 );
      const Sphere b ( a );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( a.equal ( a ) );
      BOOST_CHECK ( a.equal ( b ) );
      BOOST_CHECK ( a.equal ( c ) );
      BOOST_CHECK ( b.equal ( c ) );
    }

    // Bounding sphere of two non-intersecting spheres.
    {
      const Sphere a ( Vec (  10, 0, 0 ), 1 );
      const Sphere b ( Vec ( -10, 0, 0 ), 1 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( Sphere ( Vec ( 0, 0, 0 ), 11 ) ) );
    }

    // Bounding sphere of non-intersecting sphere and point.
    {
      const Sphere a ( Vec ( 10, 0, 0 ), 2 );
      const Vec b ( -10, 0, 0 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( Sphere ( Vec ( 1, 0, 0 ), 11 ) ) );
    }

    // Bounding sphere of two points.
    {
      const Vec a (  10, 0, 0 );
      const Vec b ( -10, 0, 0 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( Sphere ( Vec ( 0, 0, 0 ), 10 ) ) );
    }

    // Bounding sphere of intersecting spheres.
    {
      const Sphere a ( Vec (  1, 0, 0 ), 10 );
      const Sphere b ( Vec ( -1, 0, 0 ), 10 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( Sphere ( Vec ( 0, 0, 0 ), 11 ) ) );
    }

    // Bounding sphere of sphere and enclosed point.
    {
      const Sphere a ( Vec ( 1, 0, 0 ), 10 );
      const Vec b ( -1, 0, 0 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( a ) );
    }

    // Bounding sphere of sphere and enclosed sphere.
    {
      const Sphere a ( Vec (  1, 0, 0 ), 10 );
      const Sphere b ( Vec ( -1, 0, 0 ),  2 );
      const Sphere c ( Sphere::bounds ( a, b ) );
      BOOST_CHECK ( c.equal ( a ) );
    }

    // Bounding box of a sphere.
    {
      const Sphere s ( Vec ( 0, 0, 0 ), 1 );
      const Vec mn ( s.boxMin() );
      const Vec mx ( s.boxMax() );
      BOOST_CHECK ( mn.equal ( Vec ( -1, -1, -1 ) ) );
      BOOST_CHECK ( mx.equal ( Vec (  1,  1,  1 ) ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test001()
{
  Tests::Usul::Math::Details::test001<long double>();
  Tests::Usul::Math::Details::test001<double>();
  Tests::Usul::Math::Details::test001<float>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test002()
{
  Tests::Usul::Math::Details::test001<double>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test003()
{
  Tests::Usul::Math::Details::test003<double>();
  Tests::Usul::Math::Details::test003<float>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test004()
{
  Tests::Usul::Math::Details::test004<long double>();
  Tests::Usul::Math::Details::test004<double>();
  Tests::Usul::Math::Details::test004<float>();
}


} // namespace Math
} // namespace Usul
} // namespace Tests
