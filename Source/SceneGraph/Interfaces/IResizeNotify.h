
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to notify about a resize.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_RESIZE_NOTIFY_H_
#define _SCENE_GRAPH_INTERFACES_RESIZE_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Matrix44.h"


namespace SceneGraph {
namespace Interfaces {


struct IResizeNotify : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IResizeNotify );

  // Id for this interface.
  enum { IID = 2745125633u };

  // Set the new size.
  virtual void      sizeSet ( double width, double height ) = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_RESIZE_NOTIFY_H_
