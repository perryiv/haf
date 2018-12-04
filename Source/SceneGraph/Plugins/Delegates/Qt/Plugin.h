
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

#ifndef _SCENE_GRAPH_QT_DELEGATE_COMPONENT_CLASS_H_
#define _SCENE_GRAPH_QT_DELEGATE_COMPONENT_CLASS_H_

#include "SceneGraph/Plugins/Delegates/Qt/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IViewCreate.h"

#include "boost/noncopyable.hpp"


namespace SceneGraph {
namespace Plugins {


class Plugin : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public Usul::Interfaces::IViewCreate
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IViewCreate IViewCreate;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Plugin );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  Plugin();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Scene graph Qt delegate"; }
  virtual void                    pluginInitialize(){}
  virtual void                    pluginFinalize(){}

  // Usul::Interfaces::IViewCreate
  virtual bool                    canCreateView ( IUnknown::RefPtr document ) const;
  virtual void                    createView ( QWidget *parent, IUnknown::RefPtr document, IUnknown::RefPtr caller );

protected: 

  // Use reference counting.
  virtual ~Plugin();
};


} // namespace Plugins
} // namespace SceneGraph


#endif // _SCENE_GRAPH_QT_DELEGATE_COMPONENT_CLASS_H_
