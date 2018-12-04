
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for destroying an object. Call this right before you delete it.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_DESTROY_OBJECT_H_
#define _USUL_INTERFACES_DESTROY_OBJECT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IDestroyObject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDestroyObject );

  // Id for this interface.
  enum { IID = 1555583550u };

  virtual void      destroyObject() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_DESTROY_OBJECT_H_
