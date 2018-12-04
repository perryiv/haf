
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
//  Interface for accepting a property editor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_PROPERTY_EDITOR_ACCEPT_H_
#define _USUL_INTERFACES_PROPERTY_EDITOR_ACCEPT_H_

#include "Usul/Interfaces/IUnknown.h"

#include "boost/any.hpp"


namespace Usul {
namespace Interfaces {


struct IPropertyEditorAccept : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPropertyEditorAccept );

  // Id for this interface.
  enum { IID = 3128983777u };

  // Accept the property editor.
  virtual void        propertyEditorAccept ( Usul::Interfaces::IUnknown::RefPtr editor ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_PROPERTY_EDITOR_ACCEPT_H_
