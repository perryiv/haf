
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compile-time assertion.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MPL_STATIC_ASSERT_H_
#define _USUL_MPL_STATIC_ASSERT_H_


namespace Usul {
namespace MPL {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic declaration but no definition. Specialization for true.
//
///////////////////////////////////////////////////////////////////////////////

template < bool T > struct StaticAssert;
template <> struct StaticAssert < true >{};


} // namespace MPL
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro for using static assertions.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_STATIC_ASSERT(exp)\
  Usul::MPL::StaticAssert<(exp)>()


#endif // _USUL_MPL_STATIC_ASSERT_H_
