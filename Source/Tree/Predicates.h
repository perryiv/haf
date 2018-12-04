
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicates for use with algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_PREDICATES_H_
#define _TREE_PREDICATES_H_

#include <string>


namespace Tree {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if the names are equal.
//
///////////////////////////////////////////////////////////////////////////////

struct EqualName
{
  EqualName ( const std::string &name ) : _name ( name ){}
  EqualName ( const EqualName &e ) : _name ( e._name ){}
  template < class NodePtr > bool operator () ( const NodePtr &node ) const
  {
    if ( true == node.valid() )
    {
      const std::string name ( node->name() );
      if ( _name == name )
      {
        return true;
      }
    }
    return false;
  }
private:
  EqualName & operator = ( const EqualName & );
  const std::string _name;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if the node has an attribute with the given name and value.
//
///////////////////////////////////////////////////////////////////////////////

struct EqualAttribute
{
  EqualAttribute ( const std::string &name, const std::string &value ) : _name ( name ), _value ( value ){}
  EqualAttribute ( const EqualAttribute &e ) : _name ( e._name ), _value ( e._value ){}
  template < class NodePtr > bool operator () ( const NodePtr &node ) const
  {
    if ( true == node.valid() )
    {
      const std::string value ( node->attribute ( _name ) );
      if ( false == value.empty() )
      {
        if ( _value == value )
        {
          return true;
        }
      }
    }
    return false;
  }
private:
  EqualAttribute & operator = ( const EqualAttribute & );
  const std::string _name;
  const std::string _value;
};


} // namespace Predicates
} // namespace Tree


#endif // _TREE_PREDICATES_H_
