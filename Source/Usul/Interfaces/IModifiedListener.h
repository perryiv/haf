
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach and Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for listening to a subject.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_MODIFIED_LISTENER_H_
#define _USUL_INTERFACES_MODIFIED_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IModifiedListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModifiedListener );

  // Id for this interface.
  enum { IID = 2380962817u };

  virtual void subjectModified ( IUnknown::RefPtr caller = IUnknown::RefPtr() ) = 0;
};


} // end namespace Interfaces
} // end namespace Usul


#endif // _USUL_INTERFACES_MODIFIED_LISTENER_H_
