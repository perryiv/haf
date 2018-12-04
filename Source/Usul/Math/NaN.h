
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The NaN function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_NOT_A_NUMBER_FUNCTION_H_
#define _USUL_MATH_NOT_A_NUMBER_FUNCTION_H_

#include "Usul/Cast/Cast.h"

#include <cmath>

///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define THROW for the various platforms.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
  #define _USUL_THROW
#elif __CYGWIN__
  #define _USUL_THROW
#elif __APPLE__
  #define _USUL_THROW
  #ifdef isnan
  #undef isnan
  #endif
#else
  #define _USUL_THROW throw()
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define Visual Studio code annotations.
//
//  These macros ensures that the function signature is the same as in 
//  Microsoft provided headers.
//
//  This is to fix a warning with Visual Studio 2008.
//  See: http://msdn.microsoft.com/en-us/library/cc664879.aspx
//
///////////////////////////////////////////////////////////////////////////////

#if defined ( _MSC_VER ) && _MSC_VER > 1400
#include <sal.h>
#define _USUL_IN _In_
#define _USUL_CHECK_RETURN_ _Check_return_
#else
#define _USUL_IN
#define _USUL_CHECK_RETURN_
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Both cmath and math.h are a pain. 
//
//  Note: Needed to add 'throws()' so that Linux gcc would accept the 
//  prototypes in a source file that also included math.h. I still think this 
//  is better than including math.h or cmath here. I've been down that road.
//
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#ifdef _MSC_VER
  _USUL_CHECK_RETURN_ _CRTIMP int __cdecl _isnan ( _USUL_IN double );
#elif __APPLE__
  int isnan ( double ) _USUL_THROW;
#elif __GNUC__
  int __isnan ( double );
#else
  int isnan ( double ) _USUL_THROW;
#endif
};


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef _USUL_THROW


namespace Usul {
namespace Math {


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is a NAN.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool nan ( long double v )
  {
  #ifdef _WIN32
    return ( 0 != ::_isnan  ( USUL_UNSAFE_CAST ( double, v ) ) );
  #elif __APPLE__
    return ( 0 !=  ::isnan  ( USUL_UNSAFE_CAST ( double, v ) ) );
  #elif __GNUC__
    return ( 0 != ::__isnan ( USUL_UNSAFE_CAST ( double, v ) ) );
  #else
    return ( 0 !=  ::isnan  ( USUL_UNSAFE_CAST ( double, v ) ) );
  #endif
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is a NAN.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool nan ( double v )
  {
  #ifdef _WIN32
    return ( 0 != ::_isnan ( v ) );
  #else
    return ( 0 !=  ::isnan ( v ) );
  #endif
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is a NAN.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool nan ( float v )
  {
  #ifdef _WIN32
    return ( 0 != ::_isnan ( USUL_UNSAFE_CAST ( double, v ) ) );
  #else
    return ( 0 !=  ::isnan ( USUL_UNSAFE_CAST ( double, v ) ) );
  #endif
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is a NAN. Using template overloads so that, 
//  if needed, the distinction can be made at the call site.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > inline bool nan ( T v );
template <> inline bool nan < long double > ( long double v )
{
  return Usul::Math::Helper::nan ( v );
}
template <> inline bool nan < double > ( double v )
{
  return Usul::Math::Helper::nan ( v );
}
template <> inline bool nan < float > ( float v )
{
  return Usul::Math::Helper::nan ( v );
}


} // namespace Math
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining the nan function for 4x4 matrix types.
// 
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_NAN_MATRIX(matrix_type) \
namespace Usul { namespace Math { inline bool nan ( const matrix_type &m ) \
{ \
  return ( \
    Usul::Math::nan ( m[ 0] ) || \
    Usul::Math::nan ( m[ 1] ) || \
    Usul::Math::nan ( m[ 2] ) || \
    Usul::Math::nan ( m[ 3] ) || \
    Usul::Math::nan ( m[ 4] ) || \
    Usul::Math::nan ( m[ 5] ) || \
    Usul::Math::nan ( m[ 6] ) || \
    Usul::Math::nan ( m[ 7] ) || \
    Usul::Math::nan ( m[ 8] ) || \
    Usul::Math::nan ( m[ 9] ) || \
    Usul::Math::nan ( m[10] ) || \
    Usul::Math::nan ( m[11] ) || \
    Usul::Math::nan ( m[12] ) || \
    Usul::Math::nan ( m[13] ) || \
    Usul::Math::nan ( m[14] ) || \
    Usul::Math::nan ( m[15] ) ); \
} }; }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining the nan function for 4D vector types.
// 
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_NAN_VEC4(vector_type) \
namespace Usul { namespace Math { inline bool nan ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::nan ( v[0] ) || \
    Usul::Math::nan ( v[1] ) || \
    Usul::Math::nan ( v[2] ) || \
    Usul::Math::nan ( v[3] ) ); \
} }; }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining the nan function for 3D vector types.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_NAN_VEC3(vector_type) \
namespace Usul { namespace Math { inline bool nan ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::nan ( v[0] ) || \
    Usul::Math::nan ( v[1] ) || \
    Usul::Math::nan ( v[2] ) ); \
} }; }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining the nan function for 2D vector types.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_NAN_VEC2(vector_type) \
namespace Usul { namespace Math { inline bool nan ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::nan ( v[0] ) || \
    Usul::Math::nan ( v[1] ) ); \
} }; }


#endif // _USUL_MATH_NOT_A_NUMBER_FUNCTION_H_
