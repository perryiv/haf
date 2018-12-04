
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating the view.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VIEW_CREATE_H_
#define _USUL_INTERFACE_VIEW_CREATE_H_

#include "Usul/Interfaces/IUnknown.h"

class QWidget;


namespace Usul {
namespace Interfaces {


struct IViewCreate : public Usul::Interfaces::IUnknown
{
  // Typedefs
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewCreate );

  // Id for this interface.
  enum { IID = 2130786586u };

  // See if we can create the view for the given document.
  virtual bool canCreateView ( IUnknown::RefPtr document ) const = 0;

  // Create the view.
  virtual void createView ( QWidget *parent,
                            IUnknown::RefPtr document,
                            IUnknown::RefPtr caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_VIEW_CREATE_H_
