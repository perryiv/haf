
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generates a globally unique identifier.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_UNIQUE_ID_CREATE_H_
#define _USUL_SYSTEM_UNIQUE_ID_CREATE_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT UniqueId
{
  static std::string create();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_UNIQUE_ID_CREATE_H_
