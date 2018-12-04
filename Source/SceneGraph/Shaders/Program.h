
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Shader program class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHADER_PROGRAM_CLASS_H_
#define _SCENE_GRAPH_SHADER_PROGRAM_CLASS_H_

#include "SceneGraph/Base/Object.h"

#include <string>

namespace SceneGraph { namespace Shaders { class Manager; } }


namespace SceneGraph {
namespace Shaders {


class SCENE_GRAPH_EXPORT Program : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( Program, SceneGraph::Base::Object );

  const std::string &       fragmentCode() { return _fragmentCode; }

  const std::string &       name() { return _name; }

  const std::string &       vertexCode() { return _vertexCode; }

protected:

  friend class SceneGraph::Shaders::Manager;

  // Construction.
  Program ( const std::string &name, 
            const std::string &vertexCode,
            const std::string &fragmentCode );

  // Use reference counting.
  virtual ~Program();

private:

  const std::string _name;
  const std::string _vertexCode;
  const std::string _fragmentCode;
};


} // namespace Shaders
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHADER_PROGRAM_CLASS_H_
