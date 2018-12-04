
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to abstract the rendering context.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_RENDERING_CONTEXT_H_
#define _SCENE_GRAPH_INTERFACES_RENDERING_CONTEXT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace SceneGraph {
namespace Interfaces {


struct IContext : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IContext );

  // Id for this interface.
  enum { IID = 2111076060u };

  // Return unique id for this context.
  virtual unsigned long       contextId() const = 0;

  // Make this context current.
  virtual void                makeContextCurrent() = 0;

  // Swap the front and back buffers.
  virtual void                swapRenderingBuffers() = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_RENDERING_CONTEXT_H_
