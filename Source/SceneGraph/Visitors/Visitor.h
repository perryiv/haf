
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_VISITOR_CLASS_H_
#define _SCENE_GRAPH_VISITOR_CLASS_H_

#include "SceneGraph/Base/TimedObject.h"
#include "SceneGraph/Common/Enum.h"
#include "SceneGraph/Common/Forward.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Math/Matrix44.h"

#include <list>


namespace SceneGraph {
namespace Visitors {


class SCENE_GRAPH_EXPORT Visitor : public SceneGraph::Base::TimedObject
{
public:

  SCENE_GRAPH_OBJECT ( Visitor, SceneGraph::Base::TimedObject );

  typedef SceneGraph::Common::VisitMode VisitMode;
  typedef SceneGraph::Base::Object::RefPtr ObjectPtr;
  typedef std::list < ObjectPtr > NodePath;
  typedef Usul::Atomic::Container < NodePath > AtomicNodePath;
  typedef Usul::Math::Matrix44d Matrix;
  typedef std::list < Matrix > MatrixStack;
  typedef Usul::Atomic::Container < MatrixStack > AtomicMatrixStack;

  // Return the visit-mode.
  VisitMode::Mode         attributeVisitMode() const;

  // Return the visit-mode.
  VisitMode::Mode         groupVisitMode() const;

  // Return the top of the matrix stack.
  Matrix                  matrixStackTop() const;

  // The current path, populated during a traversal, otherwise empty.
  NodePath                nodePath();

  // Reset the visitor for use in the calling thread.
  virtual void            reset();

  // Visit nodes.
  virtual void            visit ( SceneGraph::Nodes::Groups::Group & );
  virtual void            visit ( SceneGraph::Nodes::Groups::Transform & );
  virtual void            visit ( SceneGraph::Nodes::Node & );
  virtual void            visit ( SceneGraph::Nodes::Shapes::Line & );
  virtual void            visit ( SceneGraph::Nodes::Shapes::Geometry & );
  virtual void            visit ( SceneGraph::Nodes::Shapes::Shape & );

  // Visit attributes.
  virtual void            visit ( const SceneGraph::State::Attributes::BaseAttribute & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Blending & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Color & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::CullFace & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Enable & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Hint & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Light & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::LightModel & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::LineWidth & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::Material & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::PolygonMode & ){}
  virtual void            visit ( const SceneGraph::State::Attributes::ShadeModel & ){}
  virtual void            visit ( SceneGraph::Shaders::Shader & ){}
  virtual void            visit ( SceneGraph::State::Attributes::Texture2 & ){}

  // Visit state.
  virtual void            visit ( SceneGraph::State::Container & );

protected:

  // Default construction.
  Visitor ( VisitMode::Mode gvm = VisitMode::COPY_CHILDREN,
            VisitMode::Mode avm = VisitMode::COPY_CHILDREN );

  // Use reference counting.
  virtual ~Visitor();

private:

  const VisitMode::Mode _groupVisitMode;
  const VisitMode::Mode _attributeVisitMode;
  AtomicNodePath _nodePath;
  AtomicMatrixStack _matrixStack;
};


} // namespace Visitors
} // namespace SceneGraph


#endif // _SCENE_GRAPH_VISITOR_CLASS_H_
