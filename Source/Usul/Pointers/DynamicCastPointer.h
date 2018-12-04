
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer that calls dynamic_cast.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DYNAMIC_CAST_POINTER_CLASS_H_
#define _USUL_DYNAMIC_CAST_POINTER_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Pointers/SmartPointer.h"


namespace Usul {
namespace Pointers {


template
<
  class T, 
  class Config
>
class DynamicCastPointer : public SmartPointer
<
  T,
  Config
>
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef SmartPointer < T, Config > BaseClass;
  typedef DynamicCastPointer < T, Config > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  DynamicCastPointer() : BaseClass ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OriginalType, class ConfigType > 
  DynamicCastPointer ( SmartPointer < OriginalType, ConfigType > &ptr ) : 
    BaseClass ( dynamic_cast < T * > ( ptr.get() ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  DynamicCastPointer ( const DynamicCastPointer &ptr ) : BaseClass ( ptr )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OriginalType, class ConfigType >
  DynamicCastPointer &operator = ( SmartPointer < OriginalType, ConfigType > &ptr )
  {
    BaseClass::operator = ( dynamic_cast < T * > ( ptr.get() ) );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another dynamic-cast-pointer. 
  //  This is needed so the compiler does not generate this function.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OriginalType, class ConfigType >
  DynamicCastPointer &operator = ( DynamicCastPointer < OriginalType, ConfigType > &ptr )
  {
    BaseClass::operator = ( dynamic_cast < T * > ( ptr.get() ) );
    return *this;
  }

private:

  // Assignment to raw pointers is not permitted.
  ThisType &operator = ( T * );
  template < class T2 > ThisType &operator = ( T2 * );

  // Do not permit "if ( !ptr )"
  bool operator ! ();

  // No automatic casting.
  operator T *();
  operator const T *() const;
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_DYNAMIC_CAST_POINTER_CLASS_H_
