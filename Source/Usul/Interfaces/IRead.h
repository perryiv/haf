
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for listening to a subject.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_READ_H_
#define _USUL_INTERFACE_READ_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IRead : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRead );

  // Id for this interface.
  enum { IID = 1594661597u };

  // Read the file.
  virtual void read ( const std::string &file, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_READ_H_
