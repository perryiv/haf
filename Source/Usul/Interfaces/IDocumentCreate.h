
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating documents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_CREATE_H_
#define _USUL_INTERFACE_DOCUMENT_CREATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IDocumentCreate : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentCreate );

  // Id for this interface.
  enum { IID = 1661325799u };

  // Can we create the document?
  virtual bool                    canCreateDocument ( const std::string &name ) const = 0;

  // Create the document.
  virtual IUnknown::RefPtr        createDocument ( IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_CREATE_H_
