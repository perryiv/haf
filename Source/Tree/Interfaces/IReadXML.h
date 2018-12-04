
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading XML and returning a new node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_LIBRARY_INTERFACES_XML_READ_H_
#define _TREE_LIBRARY_INTERFACES_XML_READ_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Types/Types.h"

#include "Tree/Node.h"

#include <iosfwd>
#include <string>


namespace Tree {
namespace Interfaces {


struct IReadXML : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadXML );

  // Id for this interface.
  enum { IID = 3679908607u };

  // Read the XML file and return the root node.
  virtual Tree::Node::RefPtr readXmlFile ( 
    const std::string &file, 
    IUnknown::RefPtr caller = IUnknown::RefPtr() ) = 0;

  // Read the XML stream and return the root node.
  virtual Tree::Node::RefPtr readXmlFile ( 
    std::istream &in, 
    Usul::Types::UInt64 streamSize,
    IUnknown::RefPtr caller = IUnknown::RefPtr() ) = 0;
};


} // End namespace Interfaces
} // End namespace Tree


#endif // _TREE_LIBRARY_INTERFACES_XML_READ_H_
