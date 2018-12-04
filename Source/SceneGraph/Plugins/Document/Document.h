
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

#ifndef _SCENE_GRAPH_DOCUMENT_CLASS_H_
#define _SCENE_GRAPH_DOCUMENT_CLASS_H_

#include "SceneGraph/Plugins/Document/CompileGuard.h"
#include "SceneGraph/Interfaces/INodeGet.h"
#include "SceneGraph/Nodes/Groups/Group.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Documents/FileDocument.h"


namespace SceneGraph {
namespace Plugins {


class Document : 
  public Usul::Documents::FileDocument,
  public SceneGraph::Interfaces::INodeGet
{
public:

  typedef Usul::Documents::FileDocument BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef SceneGraph::Nodes::Groups::Group Group;
  typedef SceneGraph::Interfaces::INodeGet INodeGet;
  typedef INodeGet::NodePtr NodePtr;

  USUL_REFERENCED_CLASS ( Document );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction
  Document ( const std::string &file = std::string() );

  // Clear the document.
  virtual void      clearObject();

  // Destroy the document.
  virtual void      destroyObject();

  // SceneGraph::Interfaces::INodeGet
  virtual NodePtr   nodeGet();

  // Read the file and add it to existing content.
  virtual void      read ( const std::string &file, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) );

  // Save this document. Default implementation simply returns false.
  virtual bool      save();

protected:

  // Use reference counting.
  virtual ~Document();

private:

  void              _destroy();

  Usul::Atomic::Object < Group::RefPtr > _scene;
};


} // namespace Plugins
} // namespace SceneGraph


#endif // _SCENE_GRAPH_DOCUMENT_CLASS_H_
