
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Basic types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASIC_TYPES_H_
#define _USUL_BASIC_TYPES_H_

#include "Usul/Config/Config.h"

#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "boost/cstdint.hpp"


namespace Usul {
namespace Types {


typedef long int            LongInt;
typedef long long           LongLong;

typedef unsigned char       UChar;
typedef unsigned short      UShort;
typedef unsigned int        UInt;
typedef unsigned long       ULong;
typedef unsigned long int   ULongInt;
typedef unsigned long long  ULongLong;

typedef boost::int8_t       Int8;
typedef boost::int16_t      Int16;
typedef boost::int32_t      Int32;
typedef boost::int64_t      Int64;

typedef boost::uint8_t      UInt8;
typedef boost::uint16_t     UInt16;
typedef boost::uint32_t     UInt32;
typedef boost::uint64_t     UInt64;

typedef float               Float32;
typedef double              Float64;
typedef long double         LongDouble;


} // namespace Types
} // namespace Usul


#endif // _USUL_BASIC_TYPES_H_
