
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry database class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Config/Config.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Node.h"
#include "Usul/Registry/Visitor.h"
#include "Usul/Threads/Guard.h"

#include <algorithm>

using namespace Usul::Registry;

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Database *Database::_persistent ( 0x0 );
Database *Database::_program    ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize singleton early before other threads start.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Life
  {
    Life()
    {
      Usul::Registry::Database::instance ( true  );
      Usul::Registry::Database::instance ( false );
    }
    ~Life()
    {
      Usul::Registry::Database::destroy ( true  );
      Usul::Registry::Database::destroy ( false );
    }
  } life;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Database::Database() : 
  _root ( new Node() )
{
  Guard guard ( _root.mutex() );
  _root.getReference()->name ( "registry" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Database::~Database()
{
  _root = Node::RefPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance. Not thread safe allocation, which is why we 
//  initialize with helper struct above.
//
///////////////////////////////////////////////////////////////////////////////

Database &Database::instance ( bool persistent )
{
  if ( true == persistent )
  {
    if ( 0x0 == _persistent )
    {
      _persistent = new Database;
    }
    return *_persistent;
  }
  else
  {
    if ( 0x0 == _program )
    {
      _program = new Database;
    }
    return *_program;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

void Database::destroy ( bool persistent )
{
  if ( true == persistent )
  {
    delete _persistent;
    _persistent = 0x0;
  }
  else
  {
    delete _program;
    _program = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the database.
//
///////////////////////////////////////////////////////////////////////////////

void Database::clear()
{
  Guard guard ( _root.mutex() );
  _root.getReference()->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Database::accept ( Usul::Registry::Visitor *v )
{
  if ( 0x0 != v )
  {
    v->visit ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Operator to return the child with the name. Creates child nodes as needed.
//
///////////////////////////////////////////////////////////////////////////////

Node &Database::operator [] ( const std::string &name )
{
  Guard guard ( _root.mutex() );
  return (*(_root.getReference()))[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Operator to return the child with the path. Creates child nodes as needed.
//
///////////////////////////////////////////////////////////////////////////////

Node &Database::operator [] ( const Node::Path &path )
{
  Guard guard ( _root.mutex() );
  return (*(_root.getReference()))[path];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Registry::Node::RefPtr Database::root()
{
  return _root;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Registry::Node::RefPtr Database::root() const
{
  return _root;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the given string to a legal tag. This function is re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

std::string Database::convertToTag ( const std::string &s ) const
{
  std::string tag ( s );

  std::replace ( tag.begin(), tag.end(), ' ',  '_' );
  std::replace ( tag.begin(), tag.end(), '/',  '_' );
  std::replace ( tag.begin(), tag.end(), '\\', '_' );
  std::replace ( tag.begin(), tag.end(), ':',  '_' );
  std::replace ( tag.begin(), tag.end(), '.',  '_' );
  std::replace ( tag.begin(), tag.end(), '-',  '_' );

  return tag;
}
