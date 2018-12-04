
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  List of lists of draw elements.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_DRAW_LISTS_CLASS_H_
#define _SCENE_GRAPH_DRAW_LISTS_CLASS_H_

#include "SceneGraph/Draw/Element.h"

#include "Usul/Atomic/Container.h"

#include <map>
#include <vector>


namespace SceneGraph {
namespace Draw {


class SCENE_GRAPH_EXPORT Lists : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( Lists, SceneGraph::Base::Object );
  typedef std::vector < Element::RefPtr > ElementList;
  typedef std::map < int, ElementList > ElementLists;
  typedef Usul::Atomic::Container < ElementLists > AtomicElementLists;

  // Default construction.
  Lists();

  // Append an element to the list.
  void                append ( int key, Element::RefPtr );

  // Clear the lists. Pass true to clear the map.
  void                clear ( bool deleteMemory = false );

  // Get a copy of the elements.
  template < class Container >
  void                elements ( int key, Container &c ) const;

  // Get a list of the map's keys.
  template < class Container >
  void                keys ( Container &c ) const;

  // Get the number of elements for the key.
  unsigned int        numElements ( int key ) const;

protected:

  // Use reference counting.
  virtual ~Lists();

private:

  AtomicElementLists _elementLists;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the elements.
//
///////////////////////////////////////////////////////////////////////////////

template < class C > void Lists::elements ( int key, C &c ) const
{
  typedef typename ElementLists::const_iterator Itr;

  // Lock the elements and get a reference.
  AtomicElementLists::Guard guard ( _elementLists.mutex() );
  const ElementLists &els ( _elementLists.getReference() );

  // Find the list.
  Itr i ( els.find ( key ) );
  if ( els.end() != i )
  {
    // Copy the list.
    const ElementList &el ( i->second );
    c.insert ( c.end(), el.begin(), el.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of the map's keys.
//
///////////////////////////////////////////////////////////////////////////////

template < class C > void Lists::keys ( C &c ) const
{
  AtomicElementLists::Guard guard ( _elementLists.mutex() );
  const ElementLists &els ( _elementLists.getReference() );
  typedef typename ElementLists::const_iterator Itr;
  for ( Itr i = els.begin(); i != els.end(); ++i )
  {
    c.insert ( c.end(), i->first );
  }
}


} // namespace Draw
} // namespace SceneGraph


#endif // _SCENE_GRAPH_DRAW_LISTS_CLASS_H_
