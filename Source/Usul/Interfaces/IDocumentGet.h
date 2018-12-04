
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_DOCUMENT_H_
#define _USUL_INTERFACES_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul { namespace Documents { class Document; } }


namespace Usul {
namespace Interfaces {


struct IDocumentGet : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef Usul::Documents::Document Document;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentGet );

  // Id for this interface.
  enum { IID = 3988015526u };

  // Return the document.
  virtual const Document *  documentGet() const = 0;
  virtual Document *        documentGet() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_DOCUMENT_H_
