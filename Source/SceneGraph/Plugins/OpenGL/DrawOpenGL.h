
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The OpenGL drawing class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OPEN_GL_DRAW_CLASS_H_
#define _SCENE_GRAPH_OPEN_GL_DRAW_CLASS_H_

#include "SceneGraph/Plugins/OpenGL/CompileGuard.h"
#include "SceneGraph/Draw/Method.h"
#include "SceneGraph/Nodes/Shapes/Geometry.h"
#include "SceneGraph/Shaders/Shader.h"

#include "Usul/Math/Matrix44.h"

#include "GLee.h"

#include <map>


namespace SceneGraph {
namespace OpenGL {


class DrawOpenGL : public SceneGraph::Draw::Method
{
public:

  SCENE_GRAPH_OBJECT ( DrawOpenGL, SceneGraph::Draw::Method );
  typedef DrawLists::ElementList ElementList;
  typedef SceneGraph::Draw::Element Element;
  typedef SceneGraph::Shaders::Shader Shader;
  typedef SceneGraph::Shaders::Program Program;
  typedef SceneGraph::State::Container StateContainer;
  typedef SceneGraph::Nodes::Shapes::Geometry::Primitives Primitives;
  typedef SceneGraph::Nodes::Shapes::Geometry::SharedPrimitives SharedPrimitives;
  typedef std::map < Program::RefPtr, GLuint > ProgramMap;
  typedef Usul::Math::Matrix44d Matrix;

  // Construction
  DrawOpenGL ( unsigned long id );

  // Draw the elements in the list.
  virtual void            draw();

  // Reset the renderer for use in the calling thread.
  virtual void            reset();

  // Visit nodes.
  virtual void            visit ( SceneGraph::Nodes::Shapes::Line & );
  virtual void            visit ( SceneGraph::Nodes::Shapes::Geometry & );

  // Visit attributes.
  virtual void            visit ( const SceneGraph::State::Attributes::Blending & );
  virtual void            visit ( const SceneGraph::State::Attributes::Color & );
  virtual void            visit ( const SceneGraph::State::Attributes::CullFace & );
  virtual void            visit ( const SceneGraph::State::Attributes::Enable & );
  virtual void            visit ( const SceneGraph::State::Attributes::Hint & );
  virtual void            visit ( const SceneGraph::State::Attributes::Light & );
  virtual void            visit ( const SceneGraph::State::Attributes::LightModel & );
  virtual void            visit ( const SceneGraph::State::Attributes::LineWidth & );
  virtual void            visit ( const SceneGraph::State::Attributes::Material & );
  virtual void            visit ( const SceneGraph::State::Attributes::PolygonMode & );
  virtual void            visit ( const SceneGraph::State::Attributes::ShadeModel & );
  virtual void            visit ( SceneGraph::Shaders::Shader & );
  virtual void            visit ( SceneGraph::State::Attributes::Texture2 & );

  // Visit state.
  virtual void            visit ( SceneGraph::State::Container & );

protected:

  // Use reference counting.
  virtual ~DrawOpenGL();

  void                    _drawElement ( Element & );
  void                    _drawElements ( ElementList & );

  GLuint                  _getProgramId ( Program::RefPtr );

  void                    _setShader ( Shader::RefPtr );
  void                    _setShaderParameters ( GLuint program );

  void                    _sortElements ( ElementList & );

private:

  const unsigned long _id;
  StateContainer::RefPtr _currentStateContainer;
  Shader::RefPtr _currentShader;
  GLuint _currentProgram;
  ProgramMap _programMap;
  SharedPrimitives::RefPtr _sharedPrimitives;
  Matrix _currentModelview;
};


} // namespace OpenGL
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OPEN_GL_DRAW_CLASS_H_
