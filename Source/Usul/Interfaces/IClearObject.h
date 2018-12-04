
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for clearing an object, which is implementation defined.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_CLEAR_OBJECT_H_
#define _USUL_INTERFACES_CLEAR_OBJECT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IClearObject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClearObject );

  // Id for this interface.
  enum { IID = 1224961186u };

  virtual void      clearObject() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_CLEAR_OBJECT_H_
