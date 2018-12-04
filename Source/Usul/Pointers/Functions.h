
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenient (and safe) reference and dereference.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_
#define _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_

#include "Usul/Config/Config.h"


namespace Usul
{
  namespace Pointers
  {
    template < class T > void reference ( T *ptr )
    {
      if ( ptr )
      {
        ptr->ref();
      }
    }
    template < class T > void unreference ( T *ptr )
    {
      if ( ptr )
      {
        ptr->unref();
      }
    }
  }
}


#endif // _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_
