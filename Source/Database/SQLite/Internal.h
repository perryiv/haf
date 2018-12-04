
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal helper functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_SQLITE_INTERNAL_HELPER_FUNCTIONS_H_
#define _DATABASE_SQLITE_INTERNAL_HELPER_FUNCTIONS_H_

#ifndef _COMPILING_DATABASE_SQLITE
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
#endif

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"

#include "sqlite3.h"

#include "boost/algorithm/string/case_conv.hpp"

#include <iostream>


namespace Database {
namespace SQLite {
namespace Helper {


///////////////////////////////////////////////////////////////////////////////
//
//  Small helper class to guard an sqlite mutex.
//
///////////////////////////////////////////////////////////////////////////////

struct Guard
{
  Guard ( ::sqlite3 *db ) : _db ( db )
  {
    if ( 0x0 != _db )
    {
#if SQLITE_THREADSAFE == 1
      ::sqlite3_mutex_enter ( ::sqlite3_db_mutex ( _db ) );
#endif
    }
  }
  ~Guard()
  {
    if ( 0x0 != _db )
    {
#if SQLITE_THREADSAFE == 1
      ::sqlite3_mutex_leave ( ::sqlite3_db_mutex ( _db ) );
#endif
    }
  }
private:
  ::sqlite3 *_db;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the error string.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string errorMessage ( ::sqlite3 *db )
{
  std::string error;
  if ( 0x0 != db )
  {
    error = sqlite3_errmsg ( db );
  }
  return error;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

inline bool isSelectStatement ( const std::string &sql )
{
  const std::string s ( boost::algorithm::to_lower_copy ( sql ) );
  if ( s.size() >= 6 )
  {
    return ( ( 's' == s[0] ) && 
             ( 'e' == s[1] ) && 
             ( 'l' == s[2] ) && 
             ( 'e' == s[3] ) && 
             ( 'c' == s[4] ) && 
             ( 't' == s[5] ) );
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function, class OutputType > 
inline void getValue ( ::sqlite3_stmt *statement, Function fun, OutputType &value, unsigned int &column )
{
  if ( 0x0 == statement )
    throw Usul::Exceptions::Error ( "5398252670", "Null statement pointer" );

  value = fun ( statement, column++ );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to call the functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > void call ( Function fun, const std::string &error = "", const std::string &sql = "", sqlite3 *database = 0x0 )
{
  // Loop until the database is not busy.
  while ( true )
  {
    // Initialize the result.
    const int UNINITIALIZED_RESULT ( 0xFFFFFFFF );
    int result ( UNINITIALIZED_RESULT );

    // Call the function and assign the result.
    result = fun();

    // Check the result.
    switch ( result )
    {
      case SQLITE_IOERR_BLOCKED:
      case SQLITE_BUSY:

        std::cout << Usul::Strings::format ( Helper::errorMessage ( database ), ". Sleeping before trying again", '\n' ) << std::flush;
        Usul::System::Sleep::milliseconds ( 100 );
        break;

      case SQLITE_OK:

        return;

      case UNINITIALIZED_RESULT:

        throw Usul::Exceptions::Error 
          ( "3972486977", "Function did not return a value" );

      default:

        throw Usul::Exceptions::Error ( error, Usul::Strings::format
          ( Helper::errorMessage ( database ), ", SQL: ", sql ) );
    }
  }
}


} // namespace Helper
} // namespace SQLite
} // namespace Database


#endif // _DATABASE_SQLITE_INTERNAL_HELPER_FUNCTIONS_H_
