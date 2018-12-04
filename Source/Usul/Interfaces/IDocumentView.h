
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a document's view.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_VIEW_H_
#define _USUL_INTERFACE_DOCUMENT_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IDocumentView : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentView );

  // Id for this interface.
  enum { IID = 2231122591u };

  // Bring this view to the front.
  virtual void          bringToFront() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_VIEW_H_
