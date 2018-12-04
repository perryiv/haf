
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tree node class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include "Tree/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Convert/Convert.h"

#include "boost/noncopyable.hpp"

#include <string>
#include <map>
#include <vector>


namespace Tree {


class TREE_EXPORT Node : 
  public Usul::Base::Object,
  public boost::noncopyable
{
public:

  // Has to come first or Node::RefPtr is actually Usul::Base::Object::RefPtr
  // when Children typedef is compiled.
  USUL_REFERENCED_CLASS ( Node );

  typedef Usul::Base::Object BaseClass;
  typedef std::map < std::string, std::string > Attributes;
  typedef std::vector < Node::RefPtr > Children;

  // Construction
  Node ( const std::string &name, const std::string &value = std::string() );
  template < class T > Node ( const std::string &name, T value );

  // Append the child.
  void                    append ( Node::RefPtr );
  template < class T > 
  Node::RefPtr            append ( const std::string &name, T value );

  // Get the attribute. Returns empty string if it's not there.
  std::string             attribute ( const std::string & ) const;

  // Set the attribute.
  void                    attribute ( const std::string &name, const std::string &value );

  // Get/set the attributes.
  Attributes              attributes() const;
  void                    attributes ( const Attributes & );

  // Get/set the children.
  Children                children() const;
  void                    children ( const Children & );

  // Clear the node.
  void                    clear();

  // Return the number of children.
  unsigned int            numChildren() const;

  // Access the value.
  std::string             value() const;
  void                    value ( const std::string & );

protected:

  // Use reference counting.
  virtual ~Node();

private:

  std::string _value;
  Attributes _attributes;
  Children _children;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Template constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline Node::Node ( const std::string &name, T value ) : 
  BaseClass ( name ),
  _value      ( Usul::Convert::Type<T,std::string>::convert ( value ) ),
  _attributes (),
  _children   ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the child.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline Node::RefPtr Node::append ( const std::string &name, T value )
{
  Node::RefPtr node ( new Node ( name, value ) );
  this->append ( node );
  return node;
}


} // namespace Tree


#endif // _TREE_NODE_H_
