
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Plugins/Document/Document.h"
#include "SceneGraph/Interfaces/IReadNode.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"

using namespace SceneGraph::Plugins;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Document, BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Document::Document ( const std::string &file ) : BaseClass ( file ),
  _scene ( new Group )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Document::~Document()
{
  Usul::Functions::noThrow ( boost::bind ( &Document::_destroy, this ), "8953586820" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_destroy()
{
  _scene = Group::RefPtr();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Document::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case SceneGraph::Interfaces::INodeGet::IID:
    return static_cast < SceneGraph::Interfaces::INodeGet * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this document.
//
///////////////////////////////////////////////////////////////////////////////

void Document::destroyObject()
{
  this->_destroy();
  BaseClass::destroyObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this document.
//
///////////////////////////////////////////////////////////////////////////////

void Document::clearObject()
{
  Group::RefPtr scene ( _scene );
  if ( true == scene.valid() )
  {
    scene->clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to find the correct reader.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct FindReader
  {
    typedef SceneGraph::Interfaces::IReadNode IReadNode;
    explicit FindReader ( const std::string &file ) : _file ( file )
    {
    }
    bool operator () ( Usul::Interfaces::IUnknown::RefPtr unknown ) const
    {
      IReadNode::QueryPtr reader ( unknown );
      if ( true == reader.valid() )
      {
        return reader->canReadSceneGraphNode ( _file );
      }
      return false;
    }
  private:
    std::string _file;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing content.
//
///////////////////////////////////////////////////////////////////////////////

void Document::read ( const std::string &file, IUnknown::RefPtr )
{
  typedef SceneGraph::Interfaces::IReadNode IReadNode;
  Helper::FindReader pred ( file );
  Usul::Plugins::Manager &pm ( Usul::Plugins::Manager::instance() );
  IReadNode::QueryPtr reader ( pm.findFirstIf ( pred ) );
  if ( false == reader.valid() )
  {
    throw Usul::Exceptions::Error ( "2638513040", Usul::Strings::format 
      ( "Failed to find a reader for file: ", file ) );
  }

  typedef SceneGraph::Nodes::Node Node;
  Node::RefPtr node ( reader->readSceneGraphNode ( file ) );
  if ( false == node.valid() )
  {
    throw Usul::Exceptions::Error ( "1534660421", Usul::Strings::format 
      ( "Null node returned when reading file: ", file ) );
  }

  Group::RefPtr scene ( _scene );
  if ( true == scene.valid() )
  {
    scene->append ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

Document::NodePtr Document::nodeGet()
{
  return Group::RefPtr ( _scene );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save this document.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::save()
{
  return true;
}
