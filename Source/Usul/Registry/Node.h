
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_NODE_CLASS_H_
#define _USUL_REGISTRY_NODE_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/String.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include "boost/noncopyable.hpp"

#include <map>
#include <string>
#include <vector>

namespace Usul { namespace Registry { class Visitor; } }


namespace Usul {
namespace Registry {


class USUL_EXPORT Node : 
  public Usul::Base::Referenced,
  public boost::noncopyable
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<std::string> Path;
  typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Node );

  // Have to declare after the smart-pointer definition.
  typedef std::map<std::string, Node::RefPtr> Children;
  typedef Children::iterator Iterator;
  typedef Children::const_iterator ConstIterator;

  // Constructor.
  Node();

  // Accept the visitor.
  void                            accept ( Visitor * );

  // Clear the node.
  void                            clear();

  // Find node with given path.  No new nodes are created.
  Node::RefPtr                    find ( const std::string & ) const;

  // Get the value.
  std::string                     get ( const std::string &defaultValue, bool setValueIfEmpty );
  std::string                     get ( const std::string &defaultValue ) const;
  template < class T > T          get ( const T &defaultValue, bool setValueIfEmpty );
  template < class T > T          get ( const T &defaultValue ) const;

  // Get copy of the children.
  Children                        children() const;

  // Get and set the name.
  void                            name ( const std::string & );
  std::string                     name() const;

  // Return the number of children.
  unsigned int                    numChildren() const;

  // Operator to return the child with the name or path. Creates child nodes as needed.
  Node &                          operator [] ( const std::string &name );
  Node &                          operator [] ( const Path &path );

  // Set the value.
  template < class T > Node &     operator = ( const T &t );

  // Set the string value.
  void                            set ( const std::string & );
  void                            set ( const char * );
  template < class T > void       set ( const T &t );

protected:

  // Use reference counting.
  virtual ~Node();

  Node::RefPtr                    _getNode ( const std::string & );

private:

  Usul::Atomic::String _name;
  Usul::Atomic::String _value;
  Usul::Atomic::Container<Children> _kids;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value. Extend formatting with overloads to "FromType".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > void Node::set ( const T &t )
{
  this->set ( Usul::Convert::Type<T,std::string>::convert ( t ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value. Extend formatting with overloads to "ToType".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > T Node::get ( const T &defaultValue ) const
{
  const std::string value ( _value );

  if ( true == value.empty() )
    return defaultValue;

  T v = defaultValue;
  Usul::Convert::Type<std::string,T>::convert ( value, v );
  return v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value. Extend formatting with overloads to "from".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > T Node::get ( const T &defaultValue, bool setValueIfEmpty )
{
  const std::string value ( _value );

  if ( ( true == value.empty() ) && ( true == setValueIfEmpty ) )
  {
    this->set ( defaultValue );
  }

  return this->get<T> ( defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > Node &Node::operator = ( const T &t )
{
  this->set ( t );
  return *this;
}


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_VISITOR_CLASS_H_
