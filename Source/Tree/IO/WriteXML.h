
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML writer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_WRITER_XML_H_
#define _TREE_WRITER_XML_H_

#include "Tree/Node.h"

#include <string>
#include <iosfwd>


namespace Tree {
namespace IO {


struct TREE_EXPORT WriteXML
{
  static void       write ( const Node::RefPtr node, 
                            const std::string &file, 
                            bool includeHeader = true );
  static void       write ( const Node::RefPtr node, 
                            std::ostream &out, 
                            bool includeHeader = true );
};


} // namespace IO
} // namespace Tree


#endif // _TREE_WRITER_XML_H_
