
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Resets a value in the destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_RESET_H_
#define _USUL_SCOPE_RESET_H_


namespace Usul {
namespace Scope {


template < typename T1, typename T2 = T1, typename T3 = T2 > struct Reset
{
  Reset ( T1 &value, const T2 &initial, const T3 &final ) : 
    _value ( value ),
    _final ( final )
  {
    _value = initial;
  }

  Reset ( T1 &value, const T2 &final ) : 
    _value ( value ),
    _final ( final )
  {
  }

  ~Reset()
  {
    _value = _final;
  }

private:

  T1 &_value;
  T2 _final;
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_RESET_H_
