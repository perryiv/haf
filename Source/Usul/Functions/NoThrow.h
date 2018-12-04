
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wraps a function call with try-catch block. 
//
//  Note: sensible output in multi-threaded environment requires first 
//  building the std::ostringstream and then pushing this all at once to 
//  std::cout.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTIONS_NO_THROW_H_
#define _USUL_FUNCTIONS_NO_THROW_H_

#include "Usul/Config/Config.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Preprocess/UniqueName.h"

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include "windows.h"
# include "eh.h"
#endif

#include <stdexcept>
#include <iostream>
#include <sstream>


namespace Usul {
namespace Functions {


///////////////////////////////////////////////////////////////////////////////
//
//  Temporarily register a new structured-exception translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  struct ScopedStructuredExceptionTranslator
  {
    ScopedStructuredExceptionTranslator() : _original ( 0x0 )
    {
      _original = ::_set_se_translator ( &ScopedStructuredExceptionTranslator::_translateException );
    }

    ~ScopedStructuredExceptionTranslator()
    {
      ::_set_se_translator ( _original );
    }

  private:

    static void _translateException ( unsigned int u, EXCEPTION_POINTERS *e )
    {
      std::ostringstream out;
      out << "Error 1153181950: Structured exception " << std::hex << u << " generated. Re-throwing as C++ exception.";
      throw std::runtime_error ( out.str() );
    }

    _se_translator_function _original;
  };
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for handling exceptions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void exceptionHandler ( const char *id, const Usul::Exceptions::Cancelled & )
  {
  }
  inline void exceptionHandler ( const char *id, const Usul::Exceptions::Error &e, std::ostream &stream = std::cout )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "7491899430" : id );
    out << ": Error " << e.id() << ": " << e.message();
    Usul::Exceptions::Error::Ptr ne ( e.nested() );
    while ( 0x0 != ne.get() )
    {
      out << ": Error " << e.id() << ": " << e.message();
      ne = ne->nested();
    }
    stream << out.str() << std::flush;
  }
  inline void exceptionHandler ( const char *id, const std::exception &e, std::ostream &stream = std::cout )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "3830268935" : id ) << ": " << e.what() << '\n';
    stream << out.str() << std::flush;
  }
  inline void exceptionHandler ( const char *id, std::ostream &stream = std::cout )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "1129262471" : id ) << ": exception generated when calling wrapped function" << '\n';
    stream << out.str() << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for try blocks.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define USUL_TRY_BLOCK \
  Usul::Functions::Helper::ScopedStructuredExceptionTranslator USUL_UNIQUE_NAME; \
  try
#else
#define USUL_TRY_BLOCK try
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macros for catch blocks.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DEFINE_CATCH_BLOCKS_OUTPUT_TO_STREAM(error_id,stream_object) \
catch ( const Usul::Exceptions::Cancelled &e ) \
{ \
  Usul::Functions::Helper::exceptionHandler ( error_id, e ); \
} \
catch ( const Usul::Exceptions::Error &e ) \
{ \
  Usul::Functions::Helper::exceptionHandler ( error_id, e, stream_object ); \
} \
catch ( const std::exception &e ) \
{ \
  Usul::Functions::Helper::exceptionHandler ( error_id, e, stream_object ); \
} \
catch ( ... ) \
{ \
  Usul::Functions::Helper::exceptionHandler ( error_id, stream_object ); \
}
#define USUL_DEFINE_CATCH_BLOCKS(error_id) \
  USUL_DEFINE_CATCH_BLOCKS_OUTPUT_TO_STREAM ( error_id, std::cout )


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function for making non-throwing function call.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > void noThrow ( F function, const char *id = "" )
{
  USUL_TRY_BLOCK
  {
    function();
  }
  USUL_DEFINE_CATCH_BLOCKS ( id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  For making non-throwing function call that returns a value.
//
///////////////////////////////////////////////////////////////////////////////

template < class F, class R > R noThrow ( F function, R errorReturn, const char *id = "" )
{
  R answer ( errorReturn );
  USUL_TRY_BLOCK
  {
    answer = function();
  }
  USUL_DEFINE_CATCH_BLOCKS ( id );
  return answer;
}


} // namespace Functions
} // namespace Usul


#endif // _USUL_FUNCTIONS_NO_THROW_H_
