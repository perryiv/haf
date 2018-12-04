
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
//  Use when listening for changes to the active view.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_ACTIVE_VIEW_LISTENER_H_
#define _USUL_INTERFACES_ACTIVE_VIEW_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IActiveViewListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveViewListener );

  // Id for this interface.
  enum { IID = 1456190886u };

  // The active document has changed.
  virtual void        activeViewChanged ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_ACTIVE_VIEW_LISTENER_H_
