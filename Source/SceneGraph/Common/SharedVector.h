
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A reference counted vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHARED_VECTOR_CLASS_H_
#define _SCENE_GRAPH_SHARED_VECTOR_CLASS_H_

#include "SceneGraph/Base/Object.h"

#include "Usul/Atomic/Container.h"

#include <vector>


namespace SceneGraph {
namespace Common {


template < class T > class SharedVector : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( SharedVector, SceneGraph::Base::Object );
  typedef T ValueType;
  typedef std::vector < ValueType > Vector;
  typedef Usul::Atomic::Container < Vector > AtomicVector;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors.
  //
  /////////////////////////////////////////////////////////////////////////////

  SharedVector() : BaseClass(),
    _v()
  {
  }
  explicit SharedVector ( const Vector &v ) : BaseClass(),
    _v ( v )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const Vector &v )
  {
    _v = v;
  }
  const AtomicVector &get() const
  {
    return _v;
  }
  AtomicVector &get()
  {
    return _v;
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor. Use reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~SharedVector()
  {
  }

private:

  AtomicVector _v;
};


} // namespace Nodes
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHARED_VECTOR_CLASS_H_
