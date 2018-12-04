
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for being a subject that is listened to.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_MODIFIED_SUBJECT_H_
#define _USUL_INTERFACES_MODIFIED_SUBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IModifiedSubject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModifiedSubject );

  // Id for this interface.
  enum { IID = 2667689130u };

  virtual void modifiedListenerAdd    ( Usul::Interfaces::IUnknown::RefPtr ) = 0;
  virtual void modifiedListenerRemove ( Usul::Interfaces::IUnknown::RefPtr ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_MODIFIED_SUBJECT_H_
