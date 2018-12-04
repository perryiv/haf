
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test functions for SceneGraph library.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Nodes/Sphere.h"
#include "SceneGraph/Nodes/Transform.h"

#include "boost/test/unit_test.hpp"


namespace Tests {
namespace SceneGraph {


///////////////////////////////////////////////////////////////////////////////
//
//  To shorten the lines below.
//
///////////////////////////////////////////////////////////////////////////////

using namespace ::SceneGraph::Nodes;
using namespace ::SceneGraph::State;
using namespace ::SceneGraph::State::Attributes;
typedef Sphere::BoundingSphere BoundingSphere;
typedef BoundingSphere::second_type::Vector Vec3;
typedef Transform::Matrix Matrix;


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test001()
{
  const double radius ( 10 );
  Sphere::RefPtr sphere ( new Sphere ( radius ) );

  {
    BOOST_CHECK_EQUAL ( radius, sphere->radiusGet() );
  }

  {
    const BoundingSphere bs ( sphere->boundingSphereGet() );
    BOOST_CHECK_EQUAL ( true, bs.first );
    BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
    BOOST_CHECK ( Vec3 ( 0, 0, 0 ).equal ( bs.second.center() ) );
  }

  {
    Group::RefPtr group ( new Group );
    group->append ( sphere );
    const BoundingSphere bs ( group->boundingSphereGet() );
    BOOST_CHECK_EQUAL ( true, bs.first );
    BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
    BOOST_CHECK ( Vec3 ( 0, 0, 0 ).equal ( bs.second.center() ) );
  }

  {
    Transform::RefPtr trans ( new Transform );
    trans->append ( sphere );
    const BoundingSphere bs ( trans->boundingSphereGet() );
    BOOST_CHECK_EQUAL ( true, bs.first );
    BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
    BOOST_CHECK ( Vec3 ( 0, 0, 0 ).equal ( bs.second.center() ) );
  }

  {
    const double x ( 10 );
    const Matrix t ( Matrix::translation ( x, 0, 0 ) );
    const Matrix r ( Matrix::rotation<90> ( Vec3 ( 0, 1, 0 ) ) );
    Transform::RefPtr trans ( new Transform ( t ) );
    trans->append ( sphere );

    {
      const BoundingSphere bs ( trans->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( x, 0, 0 ).equal ( bs.second.center() ) );
    }

    {
      trans->matrixSet ( t * r );
      const BoundingSphere bs ( trans->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( x, 0, 0 ).equal ( bs.second.center() ) );
    }

    {
      trans->matrixSet ( r * t );
      const BoundingSphere bs ( trans->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( 0, 0, -x ).equal ( bs.second.center() ) );
    }

    {
      Transform::RefPtr parent ( new Transform );
      parent->append ( trans );
      parent->matrixSet ( t );
      const BoundingSphere bs ( parent->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( x, 0, -x ).equal ( bs.second.center() ) );
    }

    {
      Transform::RefPtr parent ( new Transform );
      parent->append ( trans );
      parent->matrixSet ( r );
      const BoundingSphere bs ( parent->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( -x, 0, 0 ).equal ( bs.second.center() ) );
    }

    {
      Transform::RefPtr parent ( new Transform );
      parent->append ( trans );
      parent->matrixSet ( r * t );
      const BoundingSphere bs ( parent->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( Vec3 ( -x, 0, -x ).equal ( bs.second.center() ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

inline void test002()
{
  Group::RefPtr group ( new Group );

  const double radius ( 10 );
  const Vec3 offset ( radius * 2, 0, 0 );
  const unsigned int num ( 4 );

  Sphere::RefPtr sphere ( new Sphere ( radius ) );
  {
    const BoundingSphere bs ( sphere->boundingSphereGet() );
    BOOST_CHECK_EQUAL ( true, bs.first );
    BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
  }

  for ( unsigned int i = 0; i < num; ++i )
  {
    Transform::RefPtr trans ( new Transform );
    const Vec3 t ( offset[0] * i, offset[1], offset[2] );
    trans->matrixSet ( Matrix::translation ( t ) );
    trans->append ( sphere );
    {
      const BoundingSphere bs ( trans->boundingSphereGet() );
      BOOST_CHECK_EQUAL ( true, bs.first );
      BOOST_CHECK_EQUAL ( radius, bs.second.radius() );
      BOOST_CHECK ( t.equal ( bs.second.center() ) );
    }
    group->append ( trans );
  }

  {
    const BoundingSphere bs ( group->boundingSphereGet() );
    BOOST_CHECK_EQUAL ( true, bs.first );
    BOOST_CHECK_EQUAL ( radius * ( num - 1 ), bs.second.center()[0] );

    const Vec3 mn ( -radius, -radius, -radius );
    const Vec3 mx ( radius + 2 * radius * ( num - 1 ), radius, radius );
    const Vec3 center ( Vec3 ( mn + mx ) /= 2.0 );
    BOOST_CHECK ( center.equal ( bs.second.center() ) );
    BOOST_CHECK_EQUAL ( mn.distance ( mx ) / 2, bs.second.radius() );
  }
}


} // namespace SceneGraph
} // namespace Tests
