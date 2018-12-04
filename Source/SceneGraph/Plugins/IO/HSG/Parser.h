
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The parser class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_IO_HSG_PARSER_CLASS_H_
#define _SCENE_GRAPH_IO_HSG_PARSER_CLASS_H_

#include "SceneGraph/Plugins/IO/HSG/CompileGuard.h"
#include "SceneGraph/Nodes/Node.h"

#include "Tree/Node.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"
#include "boost/function.hpp"

#include <iosfwd>
#include <map>
#include <string>


namespace SceneGraph {
namespace IO {
namespace HSG {


class Parser : public boost::noncopyable
{
public:

  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef SceneGraph::Nodes::Node SceneNode;
  typedef Tree::Node TreeNode;
  typedef boost::function1 < SceneNode::RefPtr, const TreeNode & > Handler;
  typedef std::map < std::string, Handler > Handlers;
  typedef std::map < std::string, SceneNode::RefPtr > Instances;

  Parser();
  ~Parser();

  SceneNode::RefPtr       read ( const std::string &, 
                                 IUnknown::RefPtr caller );

  SceneNode::RefPtr       read ( std::istream &, 
                                 Usul::Types::UInt64 streamSize, 
                                 IUnknown::RefPtr caller );

protected:

  SceneNode::RefPtr       _buildGeometry  ( const TreeNode &treeNode );
  SceneNode::RefPtr       _buildGroup     ( const TreeNode &treeNode );
  SceneNode::RefPtr       _buildInstance  ( const TreeNode &treeNode );
  SceneNode::RefPtr       _buildLine      ( const TreeNode &treeNode );
  SceneNode::RefPtr       _buildSphere    ( const TreeNode &treeNode );
  SceneNode::RefPtr       _buildTransform ( const TreeNode &treeNode );

private:

  Usul::Threads::Check _threadCheck;
  Handlers _handlers;
  Instances _instances;
};


} // namespace HSG
} // namespace IO
} // namespace SceneGraph


#endif // _SCENE_GRAPH_IO_HSG_PARSER_CLASS_H_
