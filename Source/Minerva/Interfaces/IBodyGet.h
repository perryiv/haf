
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the body.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_INTERFACES_BODY_H_
#define _MINERVA_INTERFACES_BODY_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Minerva { class Body; }


namespace Minerva {
namespace Interfaces {


struct IBodyGet : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBodyGet );

  // Id for this interface.
  enum { IID = 4024144186u };

  // Return the body.
  virtual const Minerva::Body *  bodyGet() const = 0;
  virtual Minerva::Body *        bodyGet() = 0;
};


} // namespace Interfaces
} // namespace Minerva


#endif // _MINERVA_INTERFACES_BODY_H_
