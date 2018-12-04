
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Ridiculously Verbose Hierarchical Objects (RVHO) writer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_WRITER_RVHO_H_
#define _TREE_WRITER_RVHO_H_

#include "Tree/Node.h"

#include <string>
#include <iosfwd>


namespace Tree {
namespace IO {


struct TREE_EXPORT WriteRVHO
{
  static void       write ( const Node::RefPtr node, const std::string &file );
  static void       write ( const Node::RefPtr node, std::ostream &out );
};


} // namespace IO
} // namespace Tree


#endif // _TREE_WRITER_RVHO_H_
