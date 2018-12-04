
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to read a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_INTERFACES_READ_H_
#define _SCENE_GRAPH_INTERFACES_READ_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Types/Types.h"

#include "SceneGraph/Nodes/Node.h"

#include <iosfwd>
#include <string>


namespace SceneGraph {
namespace Interfaces {


struct IReadNode : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef SceneGraph::Nodes::Node::RefPtr NodePtr;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadNode );

  // Id for this interface.
  enum { IID = 2512313497u };

  // See if the given file can be read.
  virtual bool                canReadSceneGraphNode ( const std::string & ) const = 0;

  // Read the given file or stream and return a new node.
  virtual NodePtr             readSceneGraphNode ( 
                                const std::string &, 
                                IUnknown::RefPtr caller = IUnknown::RefPtr() ) = 0;
  virtual NodePtr             readSceneGraphNode ( 
                                std::istream &, 
                                Usul::Types::UInt64 streamSize,
                                IUnknown::RefPtr caller = IUnknown::RefPtr() ) = 0;
};


} // End namespace Interfaces
} // End namespace SceneGraph


#endif // _SCENE_GRAPH_INTERFACES_READ_H_
