
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_IO_DAE_COMPONENT_CLASS_H_
#define _SCENE_GRAPH_IO_DAE_COMPONENT_CLASS_H_

#include "SceneGraph/Plugins/IO/DAE/CompileGuard.h"
#include "SceneGraph/Interfaces/IReadNode.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentFilters.h"

#include "boost/noncopyable.hpp"


namespace SceneGraph {
namespace IO {
namespace DAE {


class SceneGraphDAE : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public Usul::Interfaces::IDocumentFilters,
  public SceneGraph::Interfaces::IReadNode
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IDocumentFilters::Filters Filters;
  typedef SceneGraph::Interfaces::IReadNode::NodePtr NodePtr;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( SceneGraphDAE );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  SceneGraphDAE();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "SceneGraph IO DAE"; }
  virtual void                    pluginInitialize(){}
  virtual void                    pluginFinalize(){}

  // Usul::Interfaces::IDocumentFilters
  virtual void                    appendDocumentFilters ( const std::string &, Filters & ) const;

  // SceneGraph::Interfaces::IReadNode
  virtual bool                    canReadSceneGraphNode ( const std::string & ) const;
  virtual NodePtr                 readSceneGraphNode ( 
                                    const std::string &file, 
                                    IUnknown::RefPtr caller = IUnknown::RefPtr() );
  virtual NodePtr                 readSceneGraphNode ( 
                                    std::istream &stream, 
                                    Usul::Types::UInt64 streamSize,
                                    IUnknown::RefPtr caller = IUnknown::RefPtr() );

protected:

  // Use reference counting.
  virtual ~SceneGraphDAE();
};


} // namespace DAE
} // namespace IO
} // namespace SceneGraph


#endif // _SCENE_GRAPH_IO_DAE_COMPONENT_CLASS_H_
