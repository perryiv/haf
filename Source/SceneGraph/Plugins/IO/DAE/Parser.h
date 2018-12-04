
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

#ifndef _SCENE_GRAPH_IO_DAE_PARSER_CLASS_H_
#define _SCENE_GRAPH_IO_DAE_PARSER_CLASS_H_

#include "SceneGraph/Plugins/IO/DAE/CompileGuard.h"
#include "SceneGraph/Nodes/Node.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

#include <iosfwd>


namespace SceneGraph {
namespace IO {
namespace DAE {


class Parser : public boost::noncopyable
{
public:

  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef SceneGraph::Nodes::Node SceneNode;

  Parser();
  ~Parser();

  SceneNode::RefPtr     read ( 
                          const std::string &, 
                          IUnknown::RefPtr caller );

  SceneNode::RefPtr     read ( 
                          std::istream &, 
                          Usul::Types::UInt64 streamSize, 
                          IUnknown::RefPtr caller );

private:

  Usul::Threads::Check _threadCheck;
};


} // namespace DAE
} // namespace IO
} // namespace SceneGraph


#endif // _SCENE_GRAPH_IO_DAE_PARSER_CLASS_H_
