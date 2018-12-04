
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that throws.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_ERROR_POLICY_H_
#define _USUL_MATH_ERROR_POLICY_H_

#include "Usul/Config/Config.h"
#include "Usul/Strings/Format.h"

#include <stdexcept>


namespace Usul {
namespace Math {


struct ErrorPolicy
{
  template < class IntegerType > static void bounds ( unsigned int id, IntegerType size, IntegerType index )
  {
    if ( index < 0 || index >= size )
    {
      throw std::runtime_error ( Usul::Strings::format ( "Error ", id, ": Index out of range" ) );
    }
  }
};


} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_ERROR_POLICY_H_
