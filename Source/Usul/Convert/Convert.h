
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macros for defining string conversions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONVERT_COMMON_CONVERSIONS_H_
#define _USUL_CONVERT_COMMON_CONVERSIONS_H_

#include "Usul/Convert/Generic.h"
#include "Usul/IO/TextReader.h"
#include "Usul/IO/TextWriter.h"
#include "Usul/Types/Types.h"

#include <algorithm>
#include <sstream>
#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for boolean.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < bool, std::string >
    {
      typedef Type < bool, std::string > ThisType;
      static void convert ( bool from, std::string &to )
      {
        to = ( ( true == from ) ? "true" : "false" );
      }
      static std::string convert ( bool from )
      {
        std::string to;
        ThisType::convert ( from, to );
        return to;
      }
    };
    template <> struct Type < std::string, bool >
    {
      typedef Type < std::string, bool > ThisType;
      static void convert ( const std::string &from, bool &to )
      {
        std::string copy ( from );
        std::transform ( copy.begin(), copy.end(), copy.begin(), ::tolower );
        to = ( ( ( "true" == copy ) || ( "1" == copy ) ) ? true : false );
      }
      static bool convert ( const std::string &from )
      {
        bool to ( false );
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for string.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < std::string, std::string >
    {
      typedef Type < std::string, std::string > ThisType;
      static void convert ( const std::string &from, std::string &to )
      {
        to = from;
      }
      static std::string convert ( const std::string &from )
      {
        return from;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for c-style strings.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < const char *, std::string >
    {
      typedef Type < const char *, std::string > ThisType;
      static void convert ( const char *from, std::string &to )
      {
        to = std::string ( ( 0x0 == from ) ? "" : from );
      }
      static std::string convert ( const char *from )
      {
        std::string to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining converters to-from std::string.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CONVERT_DEFINE_STRING_CONVERTER(the_type) \
namespace Usul \
{ \
  namespace Convert \
  { \
    template <> struct Type < the_type, std::string > \
    { \
      typedef Type < the_type, std::string > ThisType; \
      static void convert ( const the_type &from, std::string &to ) \
      { \
        std::ostringstream out; \
        Usul::IO::Text::Writer::write ( out, from ); \
        to = out.str(); \
      } \
      static std::string convert ( const the_type &from ) \
      { \
        std::string to; \
        ThisType::convert ( from, to ); \
        return to; \
      } \
    }; \
    template <> struct Type < std::string, the_type > \
    { \
      typedef Type < std::string, the_type > ThisType; \
      static void convert ( const std::string &from, the_type &to ) \
      { \
        std::istringstream in ( from ); \
        Usul::IO::Text::Reader::read ( in, to ); \
      } \
      static the_type convert ( const std::string &from ) \
      { \
        the_type to = the_type(); \
        ThisType::convert ( from, to ); \
        return to; \
      } \
    }; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macros for defining converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_2(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_2 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_2 ( the_type ); \
  USUL_CONVERT_DEFINE_STRING_CONVERTER ( the_type )

#define USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_3(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_3 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_3 ( the_type ); \
  USUL_CONVERT_DEFINE_STRING_CONVERTER ( the_type )

#define USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_4(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( the_type ); \
  USUL_CONVERT_DEFINE_STRING_CONVERTER ( the_type )

#define USUL_CONVERT_DEFINE_STRING_CONVERTER_MATRIX_4_4(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_MATRIX_4_4 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_MATRIX_4_4 ( the_type ); \
  USUL_CONVERT_DEFINE_STRING_CONVERTER ( the_type )


///////////////////////////////////////////////////////////////////////////////
//
//  Define standard types.
//
///////////////////////////////////////////////////////////////////////////////

USUL_CONVERT_DEFINE_STRING_CONVERTER ( char );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( short );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( int );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( long );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::LongLong );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::UChar );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::UShort );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::UInt );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::ULong );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::ULongLong );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::Float32 );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::Float64 );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Types::LongDouble );


#endif // _USUL_CONVERT_COMMON_CONVERSIONS_H_
