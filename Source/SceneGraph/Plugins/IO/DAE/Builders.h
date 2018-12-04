
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scene builder classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_IO_DAE_BUILDER_CLASSES_H_
#define _SCENE_GRAPH_IO_DAE_BUILDER_CLASSES_H_

#include "SceneGraph/Plugins/IO/DAE/CompileGuard.h"
#include "SceneGraph/Nodes/Node.h"

#include "Tree/Node.h"

#include "Usul/Base/Referenced.h"

#include "boost/tuple/tuple.hpp"

#include <map>
#include <vector>


namespace SceneGraph {
namespace IO {
namespace DAE {


///////////////////////////////////////////////////////////////////////////////
//
//  Base builder class.
//
///////////////////////////////////////////////////////////////////////////////

class BaseBuilder : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;
  USUL_REFERENCED_CLASS ( BaseBuilder );
  typedef SceneGraph::Nodes::Node SceneNode;
  typedef Tree::Node XmlNode;
  typedef boost::tuples::tuple < XmlNode::RefPtr, SceneNode::RefPtr > NodeData;
  typedef std::vector < NodeData > NodeSequence;
  typedef std::map < std::string, NodeSequence > NodeMap;
  typedef std::map < std::string, NodeMap > NodeMaps;
  typedef std::map < std::string, BaseBuilder::RefPtr > Builders;

  virtual SceneNode::RefPtr build ( const XmlNode::RefPtr &, NodeMaps &, Builders & )
  {
    return SceneNode::RefPtr();
  }

protected:

  BaseBuilder()
  {
  }

  virtual ~BaseBuilder()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete builder class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class Builder : public BaseBuilder
{
public:

  typedef BaseBuilder BaseClass;
  USUL_REFERENCED_CLASS ( Builder );

  Builder ( Function f ) : _function ( f )
  {
  }

  virtual SceneNode::RefPtr build ( const XmlNode::RefPtr &node, NodeMaps &nodes, Builders &builders )
  {
    return _function ( node, nodes, builders );
  }

protected:

  virtual ~Builder()
  {
  }

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for making builders.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > inline BaseBuilder::RefPtr makeBuilder ( Function fun )
{
  typedef Builder < Function > BuilderType;
  return BaseBuilder::RefPtr ( new BuilderType ( fun ) );
}


}; // namespace DAE
}; // namespace IO
}; // namespace SceneGraph


#endif // _SCENE_GRAPH_IO_DAE_BUILDER_CLASSES_H_
