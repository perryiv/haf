
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A helper class for pushing and popping elements of a stack.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_COMMON_SCOPED_STACK_H_
#define _SCENE_GRAPH_COMMON_SCOPED_STACK_H_

#include "SceneGraph/Config/Config.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Shared matrix class.
//
///////////////////////////////////////////////////////////////////////////////

namespace SceneGraph {
namespace Common {


template < class ContainerType > struct ScopedStack
{
  typedef ContainerType Container;
  typedef typename Container::ElementType ElementType;

  ScopedStack ( Container &c, const ElementType &e ) : _c ( c )
  {
    _c.push_back ( e );
  }

  ~ScopedStack()
  {
    _c.pop_back();
  }

  const ElementType &top() const
  {
    return _c.back();
  }

private:

  Container &_c;
};


} // namespace Common
} // namespace SceneGraph


#endif // _SCENE_GRAPH_COMMON_SCOPED_STACK_H_
