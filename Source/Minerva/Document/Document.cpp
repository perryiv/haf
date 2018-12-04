
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Document.h"
#include "Minerva/Document/Constants.h"
#include "Minerva/Document/Viewer.h"

#include "SceneGraph/Nodes/Line.h"
#include "SceneGraph/State/Attributes/Attributes.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/NaN.h"

#include "boost/bind.hpp"

using namespace Minerva;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Document, BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Make needed local functions.
//
///////////////////////////////////////////////////////////////////////////////

USUL_DECLARE_NAN_VEC3 ( Usul::Math::Vec3d );
USUL_DECLARE_NAN_MATRIX ( Usul::Math::Matrix44d );


///////////////////////////////////////////////////////////////////////////////
//
//  The two colors for our in space and close to the ground.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const Usul::Math::Vec4f nearColor ( 120.0f / 255, 240.0f / 255, 120.0f / 255, 1.0f );
  const Usul::Math::Vec4f farColor ( 0, 0, 0, 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Document::Document ( const std::string &file ) : BaseClass ( file ),
  _root ( new Group ),
  _body ( new Body ),
  _pitchAngle ( 0 ),
  _pitchArmLength ( 0 ),
  _zTranslation ( 0 ),
  _rotation ( Matrix::getIdentity() ),
  _backgroundColors ( BackgroundColors ( Detail::nearColor, Detail::farColor ) )
{
  _root.getReference()->append ( _body.getReference()->scene() );

  // Initialze navigation members.
  this->navigationReset();

  // We want to start a little farther away and zoom in.
  _zTranslation.getReference() *= 2.0;

  // Twist it a little.
  Usul::Math::Vec3d axis ( -1, 1, 0 );
  axis.normalize();
  _rotation = Matrix::rotation ( -Usul::Math::PI_OVER_2, axis );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Document::~Document()
{
  Usul::Functions::noThrow ( boost::bind ( &Document::_destroy, this ), "9847544510" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_destroy()
{
  _root = Group::RefPtr();

  Body::RefPtr body ( _body.fetchAndStore ( Body::RefPtr() ) );
  if ( true == body.valid() )
    body->destroyObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Document::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IViewCreate::IID:
    return static_cast < Usul::Interfaces::IViewCreate * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this document.
//
///////////////////////////////////////////////////////////////////////////////

void Document::destroyObject()
{
  this->_destroy();
  BaseClass::destroyObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this document.
//
///////////////////////////////////////////////////////////////////////////////

void Document::clearObject()
{
  Body::RefPtr body ( _body );
  if ( true == body.valid() )
    body->clearObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the view.
//
///////////////////////////////////////////////////////////////////////////////

void Document::createView ( QWidget *parent, IUnknown::RefPtr document, IUnknown::RefPtr caller )
{
  // Make the viewer.
  Minerva::Viewer::RefPtr viewer ( new Minerva::Viewer ( parent, Document::RefPtr ( this ) ) );

  // Queue some render requests.
  const Matrix nav ( viewer->navigationMatrixGet() );
  for ( unsigned int i = 0; i < 30; ++i )
    viewer->renderJobAdd ( nav, 1000 / Constants::TARGET_FRAME_RATE );

  // This will queue even more. The desired effect is a pause followed 
  // by an animation to the "view all" position.
  viewer->viewAll();
}


///////////////////////////////////////////////////////////////////////////////
//
//  This instance can only create a view for itself.
//  This function if framework glue.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::canCreateView ( IUnknown::RefPtr document ) const
{
  const IUnknown::NoDeleteQueryPtr me ( const_cast < Document * > ( this ) );
  return ( document.get() == me.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the scene visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Document::accept ( SceneVisitor &v )
{
  Group::RefPtr root ( _root );
  if ( true == root.valid() )
  {
    root->accept ( v );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the mouse-click with the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool intersect ( const Usul::Math::Vec2i &mi,
                          const Usul::Math::Vec2i &wsi,
                          const Document::Matrix &imp, 
                          const Document::Matrix &m, 
                          const Usul::Math::Sphered &sphere,
                          Usul::Math::Vec3d &pt )
  {
    typedef Usul::Math::Vec2i Vec2i;
    typedef Usul::Math::Vec2d Vec2d;
    typedef Usul::Math::Vec3d Vec3d;

    // Normalize mouse point.
    const Vec2d  md (  mi[0],  mi[1] );
    const Vec2d wsd ( wsi[0], wsi[1] );
    const double nx (   ( ( 2.0 * ( md[0] / wsd[0] ) ) - 1.0 ) );
    const double ny ( - ( ( 2.0 * ( md[1] / wsd[1] ) ) - 1.0 ) );

    // Calculate two points on line into the scene.
    const Vec3d a ( imp * Vec3d ( nx, ny,  1 ) );
    const Vec3d b ( imp * Vec3d ( nx, ny, -1 ) );
    if ( ( true == Usul::Math::nan ( a ) ) || ( true == Usul::Math::nan ( b ) ) )
      return false;

    // Intersect with sphere.
    double t1 ( 0 ), t2 ( 0 );
    const unsigned int numHits ( sphere.intersect ( a, b, t1, t2 ) );
    if ( 2 != numHits )
      return false;

    // Calculate both intersection points.
    const Vec3d pt1 ( a + ( Vec3d ( b - a ) * t1 ) );
    const Vec3d pt2 ( a + ( Vec3d ( b - a ) * t2 ) );

    // Put them in world space.
    const Vec3d w1 ( m * pt1 );
    const Vec3d w2 ( m * pt2 );

    // Return closest intersection point. Test in world space.
    pt = ( ( w1[2] > w2[2] ) ? pt1 : pt2 );
    return true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return current navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

Document::Matrix Document::navigationMatrix() const
{
  // Get the matrices.
  const Matrix rot   ( _rotation );
  const Matrix pitch ( this->_calculatePitchMatrix() );
  const Matrix zoom  ( Matrix::translation ( 0, 0, this->zoomGet() ) );

  // Return new matrix.
  const Matrix mat ( zoom * pitch * rot );
  return mat;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation members.
//
///////////////////////////////////////////////////////////////////////////////

void Document::navigationReset()
{
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return;

  const double radius ( body->ellipse().minRadius() );
  _pitchArmLength = radius;
  this->zoomSet ( -1.5 * radius );

  _pitchAngle = 0;
  _rotation = Matrix::getIdentity();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the mouse event.
//
///////////////////////////////////////////////////////////////////////////////

void Document::handleMouseMove ( const MouseEventData &data )
{
  // Get current navigation matrix.
  const Matrix current ( this->navigationMatrix() );

  // Update the members.
  this->_updateRotation ( data, current );
  this->_updatePitch    ( data, current );
  this->_updateZoom     ( data, current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the delta-rotation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_updateRotation
  ( const MouseEventData &data, const Matrix &current )
{
  typedef Usul::Math::Vec2i Vec2i;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Sphered Sphered;

  // Are the correct buttons down?
  if ( false == data.get<3>() )
    return;

  // Calculate matrix.
  const Matrix m ( current );
  const Matrix p ( data.get<6>() );
  const Matrix mp ( p * m );
  const Matrix imp ( mp.inverse() );

  // Get data.
  const Vec2i lmi ( data.get<0>() );
  const Vec2i cmi ( data.get<1>() );
  const Vec2i wsi ( data.get<2>() );

  // Get sphere at origin because the line will be transformed into that 
  // space before the intersection test. Also, we do not want a bounding 
  // sphere, which may be bigger than the body, but a "tight" sphere.
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return;
  const double radius ( body->ellipse().minRadius() );
  const Sphered sphere ( Vec3d ( 0, 0, 0 ), radius );

  // Intersect last mouse.
  Vec3d pt0 ( 0, 0, 0 );
  if ( false == Helper::intersect ( lmi, wsi, imp, m, sphere, pt0 ) )
    return;

  // Intersect current mouse.
  Vec3d pt1 ( 0, 0, 0 );
  if ( false == Helper::intersect ( cmi, wsi, imp, m, sphere, pt1 ) )
    return;

  // Make vectors from center to points.
  const Vec3d c ( sphere.center() );
  Vec3d vc0 ( pt0 - c   );
  Vec3d vc1 ( pt1 - c   );
  Vec3d v01 ( pt1 - pt0 );

  // Get the angle between them.
  double radians ( vc0.angle ( vc1 ) );

  // The cross product is the axis to rotate about.
  Vec3d axis ( vc0.cross ( v01 ) );
  axis.normalize();

  // Calculate incremental rotation.
  const Matrix answer ( Matrix::rotation ( radians, axis ) );
  const bool valid ( false == Usul::Math::nan ( answer ) );

  // Update member.
  if ( true == valid )
  {
    Guard guard ( _rotation.mutex() );
    Matrix &rot ( _rotation.getReference() );
    rot = rot * answer;
  }

#if 0

  // Print info.
  std::vector<char> buffer ( 2048, '\0' );
  ::sprintf_s ( &buffer[0], buffer.size() - 1,  "  pt0: %18.6f %18.6f %18.6f",  pt0[0],  pt0[1],  pt0[2] ); ::OutputDebugString ( &buffer[0] );
  ::sprintf_s ( &buffer[0], buffer.size() - 1,  ", pt1: %18.6f %18.6f %18.6f",  pt1[0],  pt1[1],  pt1[2] ); ::OutputDebugString ( &buffer[0] );
  ::sprintf_s ( &buffer[0], buffer.size() - 1, ", axis: %18.6f %18.6f %18.6f", axis[0], axis[1], axis[2] ); ::OutputDebugString ( &buffer[0] );
  ::OutputDebugString ( "\n" );

#endif

#if 0

  // To see the lines...
  using namespace SceneGraph::Nodes;
  using namespace SceneGraph::State;
  using namespace SceneGraph::State::Attributes;
  axis *= sphere.radius();
  Group::RefPtr group ( new Group );
  StateSet::RefPtr state ( group->stateSet() );
  state->attributeAdd ( new LineWidth ( 2.0 ) );
  state->attributeAdd ( new Color3 ( 1, 0, 0 ) );
  Line::RefPtr line0 ( new Line ( pt0, c ) );
  Line::RefPtr line1 ( new Line ( pt1, c ) );
  Line::RefPtr line2 ( new Line ( axis, c ) );
  line0->contributeToBoundsSet ( false );
  line1->contributeToBoundsSet ( false );
  line2->contributeToBoundsSet ( false );
  group->append ( line0 );
  group->append ( line1 );
  group->append ( line2 );
  Group::RefPtr root ( _root );
  root->remove ( 1 );
  root->append ( group );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the zoom.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_updateZoom
  ( const MouseEventData &data, const Matrix &current )
{
  typedef Usul::Math::Vec2i Vec2i;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;

  // Are the correct buttons down?
  if ( false == data.get<5>() )
    return;

  // Get change in mouse's y-coordinate.
  const Vec2i lmi ( data.get<0>() );
  const Vec2i cmi ( data.get<1>() );
  const double dm ( cmi[1] - lmi[1] );
  if ( 0 == dm )
    return;

  // Move a percentage of distance for every change in y-coordinate of mouse.
  this->zoomFraction ( dm * 0.0025 );

  // Keep it in range.
  this->zoomClamp();

#if 0

  std::vector<char> buffer ( 2048, '\0' );
  ::sprintf_s ( &buffer[0], buffer.size() - 1,      " dm: %0.6f", dm );     ::OutputDebugString ( &buffer[0] );
  ::sprintf_s ( &buffer[0], buffer.size() - 1,   ", dist: %0.6f", dist );   ::OutputDebugString ( &buffer[0] );
  ::sprintf_s ( &buffer[0], buffer.size() - 1,     ", dz: %0.6f", dz );     ::OutputDebugString ( &buffer[0] );
  ::sprintf_s ( &buffer[0], buffer.size() - 1, ", zTrans: %0.6f", zTrans ); ::OutputDebugString ( &buffer[0] );
  ::OutputDebugString ( "\n" );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the pitch.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_updatePitch
  ( const MouseEventData &data, const Matrix &current )
{
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;

  // Are the correct buttons down?
  if ( false == data.get<4>() )
    return;

  // Get body's radii.
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return;

  // Get change in mouse's y-coordinate.
  const Vec2i lmi ( data.get<0>() );
  const Vec2i cmi ( data.get<1>() );
  const double dm ( lmi[1] - cmi[1] );
  if ( 0 == dm )
    return;

  // The angle is a function of the change in the mouse's y-coordinate.
  double degrees ( _pitchAngle );
  degrees -= ( dm / 4 );

  // Constrain to this range.
  degrees = Usul::Math::clamp ( degrees, -90.0, 0.0 );

  // Save for next time.
  _pitchAngle = degrees;

#if 0

  // Print info.
  std::vector<char> buffer ( 2048, '\0' );
  ::sprintf_s ( &buffer[0], buffer.size() - 1, "degrees: %12.6f", degrees ); ::OutputDebugString ( &buffer[0] );
  ::OutputDebugString ( "\n" );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate new pitch matrix.
//
///////////////////////////////////////////////////////////////////////////////

Document::Matrix Document::_calculatePitchMatrix() const
{
  typedef Usul::Math::Vec3d Vec3d;

  // Starting at the origin, we move the body into the screen so that the 
  // surface of the body is at the origin.
  const Matrix t ( Matrix::translation ( 0, 0, -_pitchArmLength ) );

  // We next rotate about the x-axis. The angle is a function of the change 
  // in the mouse's y-coordinate. We constrain the rotation to [0,90].
  double degrees ( _pitchAngle );
  const double radians ( degrees * Usul::Math::DEG_TO_RAD );
  const Matrix r ( Matrix::rotation ( radians, Vec3d ( 1, 0, 0 ) ) );

  // Calculate the resultant matrix and return it.
  const Matrix m ( r * t );
  return m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f Document::backgroundColor ( const Matrix &m ) const
{
  typedef Usul::Math::Vec3d Vec3d;

  // Get extreme colors.
  const BackgroundColors bg ( _backgroundColors );
  QColor nc ( 0, 60, 245, 255 );

  // Get the body's radius.
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return bg[0];
  const double radius ( body->ellipse().minRadius() );

  // Get center of body in world space.
  const Vec3d center ( m * Vec3d ( 0, 0, 0 ) );

  // Define min and max distance. Making the atmosphere 120 miles, and then 
  // normalizing with respect to the ratio of our radius with earth's.
  const double minDist ( 0 );
  const double maxDist ( ( 120.0 * 1609.344 ) * ( radius / 6378137.0 ) );

  // Get distance to ground and keep it in range.
  double dist ( center.distance ( Vec3d ( 0, 0, 0 ) ) - radius );
  dist = Usul::Math::absolute ( dist );
  dist = Usul::Math::clamp ( dist, minDist, maxDist );

  // Get percentage of distance to ground. Keep it in range.
  float percent ( static_cast < float > ( 1.0 - ( dist / ( maxDist - minDist ) ) ) );
  percent = Usul::Math::clamp ( percent, 0.0f, 1.0f );

  // Blend colors in hsv space.
  const QColor c ( QColor::fromHsvF 
    ( nc.hueF(), nc.saturationF(), nc.valueF() * percent, nc.alphaF() ) );

  // Return color as rgba.
  Vec4f rgba ( c.redF(), c.greenF(), c.blueF(), c.alphaF() );
  return rgba;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the zoom.
//
///////////////////////////////////////////////////////////////////////////////

double Document::zoomGet() const
{
  return _zTranslation;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the zoom.
//
///////////////////////////////////////////////////////////////////////////////

void Document::zoomSet ( double z )
{
  _zTranslation = z;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Zoom in/out a fraction of the range.
//
///////////////////////////////////////////////////////////////////////////////

void Document::zoomFraction ( double percent )
{
  typedef Usul::Math::Vec3d Vec3d;

  // Get the body's radius.
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return;
  const double radius ( body->ellipse().minRadius() );

  // Get body's center in world space.
  const Matrix current ( this->navigationMatrix() );
  const Vec3d center ( current * Vec3d ( 0, 0, 0 ) );

  // Get distance to center from eye, minus the radius.
  // This should get smaller as we get closer.
  const double dist ( center.distance ( Vec3d ( 0, 0, 0 ) ) - radius );
  if ( 0 == dist )
    return;

  // Move a percentage of distance for every change in y-coordinate of mouse.
  const double dz ( percent * dist );
  Guard guard ( _zTranslation.mutex() );
  double &zTrans ( _zTranslation.getReference() );
  zTrans += dz;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the zoom to the allowed range.
//
///////////////////////////////////////////////////////////////////////////////

void Document::zoomClamp()
{
  Body::RefPtr body ( _body );
  if ( false == body.valid() )
    return;
  const double radius ( body->ellipse().minRadius() );

  Guard guard ( _zTranslation.mutex() );
  double &zTrans ( _zTranslation.getReference() );
  zTrans = Usul::Math::clamp ( zTrans, -5.0 * radius, 0.0 );
}
