
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu related interfaces
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_MENU_H_
#define _USUL_INTERFACES_MENU_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


///////////////////////////////////////////////////////////////////////////////
//
//  Menubar interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IMenubar : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenubar );

  // Id for this interface.
  enum { IID = 2255292195u };

  // Append the menu button.
  virtual void                appendButton ( const std::string &path, IUnknown::RefPtr command, const std::string &icon = std::string(), char delimiter = '|' ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_MENU_H_
