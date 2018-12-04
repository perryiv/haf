
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  So that c-style casts are easy to find.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CAST_H_
#define _USUL_CAST_H_


namespace Usul
{
  namespace Cast
  {
    template < class ToType, class FromType > 
    inline ToType unsafe ( FromType from )
    {
      return ( ( ToType ) from );
    };
  }
}


#define USUL_UNSAFE_CAST(to_type,from_object)\
  Usul::Cast::unsafe < to_type > ( from_object )


#endif // _USUL_CAST_H_
