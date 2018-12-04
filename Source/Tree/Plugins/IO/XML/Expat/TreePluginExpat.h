
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_IO_XML_EXPAT_COMPONENT_CLASS_H_
#define _TREE_IO_XML_EXPAT_COMPONENT_CLASS_H_

#include "Tree/Plugins/IO/XML/Expat/CompileGuard.h"
#include "Tree/Interfaces/IReadXML.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "boost/noncopyable.hpp"


namespace Tree {
namespace Plugins {
namespace IO {


class TreePluginExpat : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public Tree::Interfaces::IReadXML
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Tree::Node::RefPtr NodePtr;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( TreePluginExpat );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  TreePluginExpat();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Tree IO Expat"; }
  virtual void                    pluginInitialize(){}
  virtual void                    pluginFinalize(){}

  // Tree::Interfaces::IReadXML
  virtual NodePtr                 readXmlFile ( 
                                    const std::string &file, 
                                    IUnknown::RefPtr caller = IUnknown::RefPtr() );
  virtual NodePtr                 readXmlFile ( 
                                    std::istream &in, 
                                    Usul::Types::UInt64 streamSize,
                                    IUnknown::RefPtr caller = IUnknown::RefPtr() );

protected: 

  // Use reference counting.
  virtual ~TreePluginExpat();
};


} // namespace IO
} // namespace Plugins
} // namespace Tree


#endif // _TREE_IO_XML_EXPAT_COMPONENT_CLASS_H_
