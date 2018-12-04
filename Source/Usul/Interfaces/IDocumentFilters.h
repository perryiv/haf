
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document's filters. 
//  Use to configure a File->Open dialog.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_FILTERS_H_
#define _USUL_INTERFACE_DOCUMENT_FILTERS_H_

#include "Usul/Interfaces/IUnknown.h"

#include <utility>
#include <list>


namespace Usul {
namespace Interfaces {


struct IDocumentFilters : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < std::string, std::string > Filter;
  typedef std::list < Filter > Filters;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentFilters );

  // Id for this interface.
  enum { IID = 4064459788u };

  // Append the filters that correspond to what this document can do.
  // Pass "open", "save", "export", "insert", etc.
  virtual void        appendDocumentFilters ( const std::string &, Filters & ) const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_FILTERS_H_
