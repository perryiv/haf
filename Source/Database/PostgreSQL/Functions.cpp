
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Miscellaneous functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/PostgreSQL/Functions.h"
#include "Database/PostgreSQL/Exception.h"

#include "boost/algorithm/string/case_conv.hpp"

#include "libpq-fe.h"

#include <sstream>

using namespace Database::PostgreSQL;


///////////////////////////////////////////////////////////////////////////////
//
//  Check the connection.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::check ( 
  const std::string &id,
  PGconn *c,
  const std::string &message,
  const std::string &database, 
  const std::string &user )
{
  // Initialize flag.
  bool error ( false );

  // Add the message if there is one.
  std::ostringstream out;
  if ( false == message.empty() )
  {
    out << message;
  }

  // Check for null connection.
  if ( 0x0 == c )
  {
    out << ". Null connection";
    error = true;
  }

  // Otherwise, check for connection error.
  else
  {
    // Get the error message.
    const char *e ( ::PQerrorMessage ( c ) );
    if ( 0x0 != e )
    {
      // If the error message is not empty...
      std::string t ( e );
      if ( false == t.empty() )
      {
        // Drop newline character.
        if ( '\n' == t.at ( t.size() - 1 ) )
        {
          t.erase ( t.size() - 1 );
        }
  
        // Append the postgres error.
        out << ". " << t;
        error = true;
      }
    }
  }

  // Throw the exception if there was an error.
  if ( true == error )
  {
    throw Exception ( id, out.str(), Exception::Ptr(), "", database, user );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the result.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::check ( 
  const std::string &id,
  PGresult *r,
  const std::string &sql,
  const std::string &message,
  const std::string &database, 
  const std::string &user )
{
  // Initialize flag.
  bool error ( false );

  // Add the message if there is one.
  std::ostringstream out;
  if ( false == message.empty() )
  {
    out << message;
  }

  // Check for null result.
  if ( 0x0 == r )
  {
    out << ". Null result";
    error = true;
  }

  // Otherwise, check for result error.
  else
  {
    // Get the error message.
    const char *e ( ::PQresultErrorMessage ( r ) );
    if ( 0x0 != e )
    {
      // If the error message is not empty...
      std::string t ( e );
      if ( false == t.empty() )
      {
        // Drop newline character.
        if ( '\n' == t.at ( t.size() - 1 ) )
        {
          t.erase ( t.size() - 1 );
        }
  
        // Append the postgres error.
        out << ". " << t;
        error = true;
      }
    }
  }

  // Throw the exception if there was an error.
  if ( true == error )
  {
    throw Exception ( id, out.str(), Exception::Ptr(), sql, database, user );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

bool isSelectStatement ( const std::string &sql )
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
