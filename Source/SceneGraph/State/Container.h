
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The state container class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_STATE_CONTAINER_CLASS_H_
#define _SCENE_GRAPH_STATE_CONTAINER_CLASS_H_

#include "SceneGraph/State/Attributes/Attributes.h"
#include "SceneGraph/Common/Enum.h"
#include "SceneGraph/Visitors/Visitor.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <map>
#include <typeinfo>
#include <vector>


namespace SceneGraph {
namespace State {


class SCENE_GRAPH_EXPORT Container : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_ATTRIBUTE ( Container, SceneGraph::Base::Object );

  // Typedefs.
  typedef SceneGraph::State::Attributes::ExclusiveAttribute ExclusiveAttribute;
  typedef SceneGraph::State::Attributes::NonExclusiveAttribute NonExclusiveAttribute;

  typedef const std::type_info * ExclusiveKey;
  typedef std::pair < const std::type_info *, unsigned int > NonExclusiveKey;

  typedef std::map < ExclusiveKey, ExclusiveAttribute::RefPtr > ExclusiveAttributes;
  typedef std::map < NonExclusiveKey, NonExclusiveAttribute::RefPtr > NonExclusiveAttributes;

  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  typedef SceneGraph::Common::VisitMode VisitMode;
  typedef SceneGraph::Visitors::Visitor Visitor;

  // Construction
  Container();

  // Pass the visitor to the state-container.
  void                    accept ( Visitor &, VisitMode::Mode );

  // Add the attribute.
  template < class Ptr > 
  void                    add ( Ptr ptr );
  void                    add ( ExclusiveAttribute::RefPtr );
  void                    add ( NonExclusiveAttribute::RefPtr );

  // Remove the attribute.
  void                    remove ( ExclusiveKey );
  void                    remove ( NonExclusiveKey );

protected:

  // Use reference counting.
  virtual ~Container();

private:

  void              _destroy();

  mutable Mutex _mutex;
  ExclusiveAttributes _ea;
  NonExclusiveAttributes _na;
};


///////////////////////////////////////////////////////////////////////////////
//
//  This resolves ambiguity for the compiler.
//
///////////////////////////////////////////////////////////////////////////////

template < class Ptr > inline void Container::add ( Ptr ptr )
{
  typedef typename Ptr::element_type ElementType;
  typedef typename ElementType::BaseClass BaseClass;
  typedef typename BaseClass::RefPtr BaseClassPtr;
  this->add ( BaseClassPtr ( ptr.get() ) );
}


} // namespace State
} // namespace SceneGraph


#endif // _SCENE_GRAPH_STATE_CONTAINER_CLASS_H_
