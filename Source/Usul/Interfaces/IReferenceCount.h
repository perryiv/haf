
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the reference count.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_REFERENCE_COUNT_H_
#define _USUL_INTERFACES_REFERENCE_COUNT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IReferenceCount : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef unsigned long RefCount;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReferenceCount );

  // Id for this interface.
  enum { IID = 1344502328u };

  virtual unsigned long     getReferenceCount() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_REFERENCE_COUNT_H_
