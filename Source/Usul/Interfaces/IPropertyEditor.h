
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
//  Interface for a property editor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_PROPERTY_EDITOR_H_
#define _USUL_INTERFACES_PROPERTY_EDITOR_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Base/Referenced.h"

#include "boost/any.hpp"

#include <string>


namespace Usul {
namespace Interfaces {


struct IPropertyEditor : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPropertyEditor );

  // Id for this interface.
  enum { IID = 1998697170u };

  // Add the property.
  virtual void        propertyAdd ( const std::string &name, 
                                    const boost::any &value,
                                    IUnknown::RefPtr callback ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_PROPERTY_EDITOR_H_
