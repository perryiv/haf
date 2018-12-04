
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Use when listening for changes to the active document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_ACTIVE_DOCUMENT_LISTENER_H_
#define _USUL_INTERFACES_ACTIVE_DOCUMENT_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IActiveDocumentListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveDocumentListener );

  // Id for this interface.
  enum { IID = 3049430086u };

  // The active document has changed.
  virtual void        activeDocumentChanged ( Usul::Interfaces::IUnknown::RefPtr oldDoc, Usul::Interfaces::IUnknown::RefPtr newDoc ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_ACTIVE_DOCUMENT_LISTENER_H_
