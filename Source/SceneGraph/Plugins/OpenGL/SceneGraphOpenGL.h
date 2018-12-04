
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

#ifndef _SCENE_GRAPH_OPENGL_COMPONENT_CLASS_H_
#define _SCENE_GRAPH_OPENGL_COMPONENT_CLASS_H_

#include "SceneGraph/Plugins/OpenGL/CompileGuard.h"
#include "SceneGraph/Interfaces/IDrawMethodCreate.h"
#include "SceneGraph/Shaders/Shader.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"

#include "boost/noncopyable.hpp"

#include <string>


namespace SceneGraph {
namespace OpenGL {


class SceneGraphOpenGL : 
  public Usul::Base::Object,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public SceneGraph::Interfaces::IDrawMethodCreate
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef SceneGraph::Interfaces::IDrawMethodCreate IDrawMethodCreate;
  typedef IDrawMethodCreate::DrawMethodPtr DrawMethodPtr;
  typedef SceneGraph::Shaders::Shader::Parameters Parameters;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( SceneGraphOpenGL );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  SceneGraphOpenGL();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "SceneGraph OpenGL"; }
  virtual void                    pluginInitialize();
  virtual void                    pluginFinalize();

  // SceneGraph::Interfaces::IDrawMethodCreate
  virtual DrawMethodPtr           createDrawMethod ( IUnknown::RefPtr );

protected: 

  // Use reference counting.
  virtual ~SceneGraphOpenGL();

private:

  void                            _addParameter ( const std::string &line, Parameters & );
  void                            _addShader ( const std::string & );
  void                            _addShaders ( const std::string & );
  void                            _addShaders();
};


} // namespace OpenGL
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OPENGL_COMPONENT_CLASS_H_
