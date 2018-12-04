
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Atomic class for container.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ATOMIC_CONTAINER_CLASS_H_
#define _USUL_ATOMIC_CONTAINER_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Object.h"


namespace Usul {
namespace Atomic {


template < class ContainerType > struct Container : public Usul::Atomic::Object < ContainerType >
{
  // Typedefs
  typedef Usul::Atomic::Object < ContainerType > BaseClass;
  typedef typename BaseClass::ValueType ValueType;
  typedef typename BaseClass::Mutex Mutex;
  typedef typename BaseClass::Guard Guard;
  typedef typename ValueType::size_type SizeType;
  typedef typename ValueType::value_type ElementType;

  // Constructors and destructor.
  Container() : BaseClass(){}
  Container ( const ValueType &v ) : BaseClass ( v ){}
  Container ( const Container &v ) : BaseClass ( v ){}

  void clear()
  {
    Guard guard ( this );
    return this->getReference().clear();
  }

  ElementType back() const
  {
    Guard guard ( this );
    return this->getReference().back();
  }

  bool empty() const
  {
    Guard guard ( this );
    return this->getReference().empty();
  }

  ElementType front() const
  {
    Guard guard ( this );
    return this->getReference().front();
  }

  void pop_back()
  {
    Guard guard ( this );
    this->getReference().pop_back();
  }

  void push_back ( const ElementType &e )
  {
    Guard guard ( this );
    this->getReference().push_back ( e );
  }

  void pop_front()
  {
    Guard guard ( this );
    this->getReference().pop_front();
  }

  void push_front ( const ElementType &e )
  {
    Guard guard ( this );
    this->getReference().push_front ( e );
  }

  SizeType size() const
  {
    Guard guard ( this );
    return this->getReference().size();
  }
};


} // namespace Atomic
} // namespace Usul


#endif // _USUL_ATOMIC_CONTAINER_CLASS_H_
