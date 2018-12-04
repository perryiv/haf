
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

#include "Tree/Plugins/IO/XML/Expat/TreePluginExpat.h"
#include "Tree/Expat/Reader.h"

#include "Usul/Plugins/Helper.h"

using namespace Tree::Plugins::IO;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TreePluginExpat, BaseClass );
USUL_PLUGIN_INITIALIZE ( TreePluginExpat );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TreePluginExpat::TreePluginExpat() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TreePluginExpat::~TreePluginExpat()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TreePluginExpat::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Tree::Interfaces::IReadXML::IID:
    return static_cast < Tree::Interfaces::IReadXML * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

std::string TreePluginExpat::pluginDescription() const
{
  return "Expat-based reader plugin for Tree";
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the new dom tree.
//
/////////////////////////////////////////////////////////////////////////////

TreePluginExpat::NodePtr TreePluginExpat::readXmlFile 
  ( const std::string &file, IUnknown::RefPtr caller )
{
  Tree::Expat::Reader reader;
  return reader.read ( file, caller );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the new dom tree.
//
/////////////////////////////////////////////////////////////////////////////

TreePluginExpat::NodePtr TreePluginExpat::readXmlFile 
  ( std::istream &in, Usul::Types::UInt64 streamSize, IUnknown::RefPtr caller )
{
  Tree::Expat::Reader reader;
  return reader.read ( in, streamSize, caller );
}