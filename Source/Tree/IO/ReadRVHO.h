
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Ridiculously Verbose Hierarchical Objects (RVHO) reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_READER_RVHO_H_
#define _TREE_READER_RVHO_H_

#include "Tree/Node.h"

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <iosfwd>


namespace Tree {
namespace IO {


struct TREE_EXPORT ReadRVHO
{
  typedef Tree::Node Node;
  typedef Node::RefPtr NodePtr;
  typedef Usul::Interfaces::IUnknown IUnknown;

  static NodePtr    read ( const std::string &file );
  static NodePtr    read ( std::istream &stream );
};


} // namespace IO
} // namespace Tree


#endif // _TREE_READER_RVHO_H_
