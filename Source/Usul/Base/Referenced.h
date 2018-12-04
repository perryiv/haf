
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counting base class with compile-time switch of base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_REFERENCE_COUNTED_CLASS_H_
#define _USUL_BASE_REFERENCE_COUNTED_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Pointers/Pointers.h"

#ifdef USUL_BASE_REFERENCED_CAREFUL
#include "Usul/Base/Ref/Careful.h"
#endif
#ifdef USUL_BASE_REFERENCED_MULTI_THREADED
#include "Usul/Base/Ref/MultiThreaded.h"
#endif
#ifdef USUL_BASE_REFERENCED_SINGLE_THREADED
#include "Usul/Base/Ref/SingleThreaded.h"
#endif


namespace Usul {
namespace Base {


class Referenced : public USUL_BASE_REFERENCED_BASE_CLASS
{
public:

  typedef USUL_BASE_REFERENCED_BASE_CLASS BaseClass;
  typedef BaseClass::RefCount RefCount;

  USUL_DECLARE_REF_POINTERS ( Referenced );

  virtual Usul::Interfaces::IUnknown::RefPtr asUnknown()
  {
    return Usul::Interfaces::IUnknown::RefPtr ( 0x0 );
  }

  void ref()
  {
    BaseClass::ref();
  }

  void unref ( bool allowDelete = true )
  {
    BaseClass::unref ( allowDelete );
  }

  RefCount refCount() const
  {
    return BaseClass::refCount();
  }

protected:

  Referenced() : BaseClass()
  {
  }

  Referenced ( const Referenced &r ) : BaseClass ( r )
  {
  }

  Referenced &operator = ( const Referenced &r )
  {
    BaseClass::operator = ( r );
    return *this;
  }

  virtual ~Referenced()
  {
  }
};


} // namespace Base
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Single macro for declaring smart-pointers and type-info.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REFERENCED_CLASS(class_name) \
  USUL_DECLARE_REF_POINTERS ( class_name ); \
  USUL_DECLARE_CAST_POINTERS ( class_name )


#endif // _USUL_BASE_REFERENCE_COUNTED_CLASS_H_
