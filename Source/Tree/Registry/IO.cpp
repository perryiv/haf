
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that reads and writes Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/Registry/IO.h"
#include "Tree/Expat/Reader.h"
#include "Tree/Registry/Builder.h"
#include "Tree/Registry/Visitor.h"
#include "Tree/IO/ReadRVHO.h"
#include "Tree/IO/WriteRVHO.h"

#include "Usul/Registry/Database.h"

using namespace Tree::Registry;


///////////////////////////////////////////////////////////////////////////////
//
//  Read the registry.
//
///////////////////////////////////////////////////////////////////////////////

void IO::read ( const std::string &file, Usul::Registry::Database &db )
{
  // Read the rvho file.
  Tree::Node::RefPtr root ( Tree::IO::ReadRVHO::read ( file ) );
  if ( false == root.valid() )
    return;

  // Build registry.
  Tree::Registry::Builder::RefPtr builder ( new Tree::Registry::Builder );
  builder->build ( root, db );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the registry.
//
///////////////////////////////////////////////////////////////////////////////

void IO::write ( const std::string &file, Usul::Registry::Database &db )
{
  // Build the node tree.
  Tree::Registry::Visitor::RefPtr visitor ( new Tree::Registry::Visitor );
  db.accept ( visitor.get() );

  // Write it to disk.
  Tree::IO::WriteRVHO::write ( visitor->root(), file );
}
