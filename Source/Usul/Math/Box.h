
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A box class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_BOX_CLASS_H_
#define _USUL_MATH_BOX_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Math {


template < typename T > class Box
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
  typedef Box < T > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Box() : _valid ( false ), _min ( 0, 0, 0 ), _max ( 0, 0, 0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from two points.
  //
  /////////////////////////////////////////////////////////////////////////////

  Box ( const Vector &v1, const Vector &v2 ) : 
    _valid ( true ), 
    _min ( Usul::Math::minimum ( v1[0], v2[0] ),
           Usul::Math::minimum ( v1[1], v2[1] ),
           Usul::Math::minimum ( v1[2], v2[2] ) ),
    _max ( Usul::Math::maximum ( v1[0], v2[0] ),
           Usul::Math::maximum ( v1[1], v2[1] ),
           Usul::Math::maximum ( v1[2], v2[2] ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Box ( const ThisType &s ) : 
    _valid ( s._valid ), _min ( s._min ), _max ( s._max )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Box &operator = ( const ThisType &s )
  {
    _valid = s._valid;
    _min = s._min;
    _max = s._max;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the min.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector &minimum() const
  {
    return _min;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the max.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector &maximum() const
  {
    return _max;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the center.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector center() const
  {
    return ( ( _min + _max ) * static_cast < T > ( 0.5 ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the valid flag.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool valid() const
  {
    return _valid;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Box &b ) const
  {
    return ( ( _min.equal ( b._min ) ) && ( _max == b._max ) );
  }
  bool operator == ( const Box &b ) const
  {
    return this->equal ( b );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a box that bounds the given geometry.
  //
  /////////////////////////////////////////////////////////////////////////////

  static Box bounds ( const Box &b, const Vector &v )
  {
    if ( false == b.valid() )
      return Box ( v, v );

    const Vector mn ( Usul::Math::minimum ( b._min[0], v[0] ),
                      Usul::Math::minimum ( b._min[1], v[1] ),
                      Usul::Math::minimum ( b._min[2], v[2] ) );
    const Vector mx ( Usul::Math::maximum ( b._max[0], v[0] ),
                      Usul::Math::maximum ( b._max[1], v[1] ),
                      Usul::Math::maximum ( b._max[2], v[2] ) );
    return Box ( mn, mx );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a box that bounds the given geometry.
  //
  /////////////////////////////////////////////////////////////////////////////

  static Box bounds ( const Vector &v, const Box &b )
  {
    return Box::bounds ( b, v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a box that bounds the given geometry.
  //
  /////////////////////////////////////////////////////////////////////////////

  static Box bounds ( const Box &b1, const Box &b2 )
  {
    if ( ( true == b1.valid() ) && ( true == b2.valid() ) )
    {
      return Box::bounds ( Box::bounds ( b1, b2.minimum() ), b2.maximum() );
    }
    else if ( ( true == b1.valid() ) && ( false == b2.valid() ) )
    {
      return b1;
    }
    else if ( ( false == b1.valid() ) && ( true == b2.valid() ) )
    {
      return b2;
    }
    else
    {
      return Box();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a transformed box.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class MatrixType > Box transform ( const MatrixType &m ) const
  {
    // Transform the eight corners.
    const Vector c1 ( m * Vector ( _min[0], _min[1], _min[2] ) );
    const Vector c2 ( m * Vector ( _max[0], _min[1], _min[2] ) );
    const Vector c3 ( m * Vector ( _max[0], _max[1], _min[2] ) );
    const Vector c4 ( m * Vector ( _min[0], _max[1], _min[2] ) );
    const Vector c5 ( m * Vector ( _min[0], _min[1], _max[2] ) );
    const Vector c6 ( m * Vector ( _max[0], _min[1], _max[2] ) );
    const Vector c7 ( m * Vector ( _max[0], _max[1], _max[2] ) );
    const Vector c8 ( m * Vector ( _min[0], _max[1], _max[2] ) );

    // Build up the new bounding box.
    Box b ( c1, c2 );
    b = Box::bounds ( b, c3 );
    b = Box::bounds ( b, c4 );
    b = Box::bounds ( b, c5 );
    b = Box::bounds ( b, c6 );
    b = Box::bounds ( b, c7 );
    b = Box::bounds ( b, c8 );

    // Return new bounding box.
    return b;
  }

private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Internal data.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool _valid;
  Vector _min;
  Vector _max;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Box < float       > Boxf;
typedef Box < double      > Boxd;
typedef Box < long double > Boxld;


} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_BOX_CLASS_H_
