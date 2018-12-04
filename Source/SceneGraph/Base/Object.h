
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Abstract base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OBJECT_CLASS_H_
#define _SCENE_GRAPH_OBJECT_CLASS_H_

#include "SceneGraph/Config/Config.h"
#include "SceneGraph/Common/Macros.h"
#include "SceneGraph/Export/Export.h"

#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"


namespace SceneGraph {
namespace Base {


class SCENE_GRAPH_EXPORT Object : 
  public Usul::Base::Referenced,
  public boost::noncopyable
{
public:

  SCENE_GRAPH_OBJECT ( Object, Usul::Base::Referenced );

protected:

  Object();
  virtual ~Object();
};


} // namespace Base
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OBJECT_CLASS_H_
