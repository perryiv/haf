
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls funtion in destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_CALL_H_
#define _USUL_SCOPE_CALL_H_

#include "Usul/Config/Config.h"

#include "boost/function.hpp"


namespace Usul {
namespace Scope {


struct Call
{
  typedef boost::function0<void> F;

  Call ( F f ) : _f ( f ), _owner ( true )
  {
  }

  Call ( const Call &c ) : _f ( c._f ), _owner ( true )
  {
    c._owner = false;
  }

  ~Call()
  {
    if ( true == _owner )
    {
      if ( _f )
      {
        _f();
      }
    }
  }

private:

  Call &operator = ( const Call & );

  F _f;
  mutable bool _owner;
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_CALL_H_
