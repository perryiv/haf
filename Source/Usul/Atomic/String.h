
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Atomic class for strings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ATOMIC_STRING_CLASS_H_
#define _USUL_ATOMIC_STRING_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Object.h"

#include <string>


namespace Usul {
namespace Atomic {


struct String : public Usul::Atomic::Object < std::string >
{
  // Typedefs
  typedef Usul::Atomic::Object < std::string > BaseClass;
  typedef BaseClass::ValueType ValueType;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef ValueType::size_type SizeType;

  // Constructors and destructor.
  String() : BaseClass(){}
  String ( const ValueType &v ) : BaseClass ( v ){}
  String ( const String &v ) : BaseClass ( v ){}

  // Assignment.
  String &operator = ( const String &rhs ) { BaseClass::operator = ( rhs ); return *this; }
  String &operator = ( const ValueType &rhs ) { BaseClass::operator = ( rhs ); return *this; }

  void clear()
  {
    Guard guard ( this->mutex() );
    return this->getReference().clear();
  }

  bool empty() const
  {
    Guard guard ( this->mutex() );
    return this->getReference().empty();
  }

  SizeType size() const
  {
    Guard guard ( this );
    return this->getReference().size();
  }
};


} // namespace Atomic
} // namespace Usul


#endif // _USUL_ATOMIC_STRING_CLASS_H_
