
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer that queries for interfaces.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_QUERY_INTERFACE_POINTER_CLASS_H_
#define _USUL_QUERY_INTERFACE_POINTER_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Pointers/SmartPointer.h"


namespace Usul {
namespace Pointers {


template
<
  class NewInterface_, 
  class Config_
>
class QueryPointer : public SmartPointer
<
  NewInterface_,
  Config_
>
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef NewInterface_ NewInterface;
  typedef Config_ Config;
  typedef SmartPointer < NewInterface, Config > BaseClass;
  typedef QueryPointer < NewInterface, Config > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  QueryPointer() : BaseClass ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from arbitrary interface pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_ > QueryPointer ( OldInterface_ *ptr ) : 
    BaseClass ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ > 
  QueryPointer ( SmartPointer < OldInterface_, ConfigType_ > &ptr ) : 
    BaseClass ( static_cast<NewInterface *> 
      ( ( ptr.valid() ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  QueryPointer ( const QueryPointer &ptr ) : BaseClass ( ptr )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ >
  QueryPointer &operator = ( SmartPointer < OldInterface_, ConfigType_ > &ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    BaseClass::operator = ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) );

    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another query-pointer. 
  //  This is needed so the compiler does not generate this function.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ >
  QueryPointer &operator = ( QueryPointer < OldInterface_, ConfigType_ > &ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    BaseClass::operator = ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) );

    return *this;
  }

private:

  // Assignment to raw pointers is not permitted.
  ThisType &operator = ( NewInterface * );
  template < class T > ThisType &operator = ( T * );

  // Do not permit "if ( !ptr )"
  bool operator ! ();

  // No automatic casting.
  operator NewInterface *();
  operator const NewInterface *() const;
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_QUERY_INTERFACE_POINTER_CLASS_H_
