
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Manager for shaders.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHADER_MANAGER_CLASS_H_
#define _SCENE_GRAPH_SHADER_MANAGER_CLASS_H_

#include "SceneGraph/Shaders/Program.h"
#include "SceneGraph/Shaders/Shader.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include <map>
#include <string>


namespace SceneGraph {
namespace Shaders {


class SCENE_GRAPH_EXPORT Manager
{
public:

  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef Shader::Parameters Parameters;
  typedef std::pair < Program::RefPtr, Parameters > ProgramAndParams;
  typedef std::map < std::string, ProgramAndParams > ProgramMap;

  // Add a program and default parameters.
  void                    addProgram ( 
                            const std::string &name, 
                            const std::string &vertexCode,
                            const std::string &fragmentCode,
                            const Parameters &params );

  // Create a new shader with the program name.
  Shader::RefPtr          createShader ( const std::string &name );

  // Return singleton instance.
  static Manager &        instance();

  // Remove programs.
  void                    removeAllPrograms();
  void                    removeProgram ( const std::string &name );

private:

  Manager();
  ~Manager();

  ProgramAndParams        _getPair ( const std::string &name ) const;

  mutable Mutex _mutex;
  ProgramMap _programs;
};


} // namespace Shaders
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHADER_MANAGER_CLASS_H_
