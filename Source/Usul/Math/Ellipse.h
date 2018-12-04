
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach and Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Ellipse class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_ELLIPSE_CLASS_H_
#define _USUL_MATH_ELLIPSE_CLASS_H_

#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Math {


template < class T > class Ellipse
{
public:

  typedef T ValueType;
  typedef ValueType value_type;
  typedef Ellipse < ValueType > ThisType;
  typedef Usul::Math::Vector2 < ValueType > Vec2;
  typedef Usul::Math::Vector3 < ValueType > Vec3;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Ellipse ( ValueType equatorialRadius, ValueType polarRadius ) :
    _equatorialRadius ( equatorialRadius ),
    _polarRadius ( polarRadius ),
    _eccentricitySquared ( 0 )
  {
    this->_updateEccentricity();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Ellipse() :
    _equatorialRadius ( 6378137.0 ),
    _polarRadius ( 6356752.3142 ),
    _eccentricitySquared ( 0 )
  {
    this->_updateEccentricity();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Ellipse ( const Ellipse &e ) :
    _equatorialRadius ( e._equatorialRadius ),
    _polarRadius ( e._polarRadius ),
    _eccentricitySquared ( e._eccentricitySquared )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Ellipse& operator = ( const Ellipse &e )
  {
    _equatorialRadius = e._equatorialRadius;
    _polarRadius = e._polarRadius;
    this->_updateEccentricity();
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return two scale factors that convert a delta-longitude and a 
  //  delta-latitude to meters. Accuracy decreases with increased distance 
  //  from the given lonLat point.
  //
  /////////////////////////////////////////////////////////////////////////////

  void localScaleLonLatToRadiiUnits ( const Vec2 &lonLat, Vec2 &scale, ValueType offset ) const
  {
    // For readability.
    const Vec3 llea ( lonLat[0], lonLat[1], 0 );

    const ValueType offsetLon ( llea[0] + offset );
    const ValueType offsetLat ( ( ( llea[1] + offset ) >  90 ) ?
       ( llea[1] - offset ) : ( llea[1] + offset ) );

    const Vec3 lleb ( offsetLon, llea[1], 0 );
    const Vec3 llec ( llea[0], offsetLat, 0 );

    Vec3 xyza ( 0, 0, 0 ), xyzb ( 0, 0, 0 ), xyzc ( 0, 0, 0 );
    this->toXYZ ( llea, xyza );
    this->toXYZ ( lleb, xyzb );
    this->toXYZ ( llec, xyzc );

    const ValueType distAB ( xyza.distance ( xyzb ) );
    const ValueType distAC ( xyza.distance ( xyzc ) );

    const ValueType lonToUnits ( distAB / offset );
    const ValueType latToUnits ( distAC / offset );

    scale.set ( lonToUnits, latToUnits );
  }

  Vec2 localScaleLonLatToRadiiUnits ( const Vec2 &lonLat, ValueType offset ) const
  {
    Vec2 scale ( 1, 1 );
    this->localScaleLonLatToRadiiUnits ( lonLat, scale, offset );
    return scale;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the radii.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vec2 radii() const
  {
    return Vec2 ( _equatorialRadius, _polarRadius );
  }

  void radii ( const Vec2 &r )
  {
    _equatorialRadius = r[0];
    _polarRadius = r[1];
    this->_updateEccentricity();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the minimum radii.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType minRadius() const
  {
    return Usul::Math::minimum ( _equatorialRadius, _polarRadius );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert the coordinates.
  //  http://www.colorado.edu/geography/gcraft/notes/datum/gif/llhxyz.gif
  //
  /////////////////////////////////////////////////////////////////////////////

  void toLLE ( ValueType x, ValueType y, ValueType z, 
               ValueType &lon, ValueType &lat, ValueType &elev, 
               ValueType scale ) const
  {
    const ValueType pr ( _polarRadius );
    const ValueType er ( _equatorialRadius );
    const ValueType es ( _eccentricitySquared );
    const ValueType prs ( pr * pr );
    const ValueType ers ( er * er );

    const ValueType p ( Usul::Math::sqrt ( ( x * x ) + ( y * y ) ) );
    const ValueType theta ( Usul::Math::atan2 ( ( z * er ), ( p * pr ) ) );
    const ValueType sinTheta ( Usul::Math::sin ( theta ) );
    const ValueType cosTheta ( Usul::Math::cos ( theta ) );
    const ValueType eDashSquared ( ( ers - prs ) / prs );

    lat = Usul::Math::atan
      ( ( z + ( eDashSquared * pr * sinTheta * sinTheta * sinTheta ) ) /
        ( p - ( es * er * cosTheta * cosTheta * cosTheta ) ) );
    lon = Usul::Math::atan2 ( y, x );

    const ValueType sinLat ( Usul::Math::sin ( lat ) );
    const ValueType N ( er / Usul::Math::sqrt (  1.0 - es * sinLat * sinLat ) );
    elev = ( p / Usul::Math::cos ( lat ) ) - N;

    lon *= scale;
    lat *= scale;
  }

  void toLLE ( const Vec3 &xyz, Vec3 &lle ) const 
  {
    const ValueType scale ( static_cast<ValueType> ( Usul::Math::RAD_TO_DEG ) );
    this->toLLE ( xyz[0], xyz[1], xyz[2], lle[0], lle[1], lle[2], scale );
  }

  Vec3 toLLE ( ValueType x, ValueType y, ValueType z ) const
  {
    Vec3 lle ( 0, 0, 0 );
    const ValueType scale ( static_cast<ValueType> ( Usul::Math::RAD_TO_DEG ) );
    this->toLLE ( x, y, z, lle[0], lle[1], lle[2], scale );
    return lle;
  }

  Vec3 toLLE ( const Vec3 &xyz ) const
  {
    Vec3 lle ( 0, 0, 0 );
    const ValueType scale ( static_cast<ValueType> ( Usul::Math::RAD_TO_DEG ) );
    this->toLLE ( xyz[0], xyz[1], xyz[2], lle[0], lle[1], lle[2], scale );
    return lle;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert the coordinates.
  //  http://www.colorado.edu/geography/gcraft/notes/datum/gif/llhxyz.gif
  //
  /////////////////////////////////////////////////////////////////////////////

  void toXYZ ( ValueType lon, ValueType lat, ValueType elev, ValueType &x, ValueType &y, ValueType &z ) const
  {
    const ValueType one ( 1 );

    lon *= static_cast < ValueType > ( Usul::Math::DEG_TO_RAD );
    lat *= static_cast < ValueType > ( Usul::Math::DEG_TO_RAD );

    const ValueType sinLat ( Usul::Math::sin ( lat ) );
    const ValueType cosLat ( Usul::Math::cos ( lat ) );
    const ValueType n ( _equatorialRadius / Usul::Math::sqrt ( one - _eccentricitySquared * sinLat * sinLat ) );
    x = ( n + elev ) * cosLat * Usul::Math::cos ( lon );
    y = ( n + elev ) * cosLat * Usul::Math::sin ( lon );
    z = ( n * ( one - _eccentricitySquared ) + elev ) * sinLat;
  }

  void toXYZ ( const Vec3 &lle, Vec3 &xyz ) const
  {
    this->toXYZ ( lle[0], lle[1], lle[2], xyz[0], xyz[1], xyz[2] );
  }

  Vec3 toXYZ ( ValueType lon, ValueType lat, ValueType elev ) const
  {
    Vec3 xyz ( 0, 0, 0 );
    this->toXYZ ( lon, lat, elev, xyz[0], xyz[1], xyz[2] );
    return xyz;
  }

  Vec3 toXYZ ( const Vec3 &lle ) const
  {
    Vec3 xyz ( 0, 0, 0 );
    this->toXYZ ( lle[0], lle[1], lle[2], xyz[0], xyz[1], xyz[2] );
    return xyz;
  }

private:

  void _updateEccentricity()
  {
    const ValueType two ( 2 );
    ValueType flattening ( ( _equatorialRadius - _polarRadius ) / _equatorialRadius );
    _eccentricitySquared = two * flattening - flattening * flattening;
  }

  ValueType _equatorialRadius;
  ValueType _polarRadius;
  ValueType _eccentricitySquared;
};

  
} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_ELLIPSE_CLASS_H_
