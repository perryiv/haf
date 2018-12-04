
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates a database connection.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/PostgreSQL/Connection.h"
#include "Database/PostgreSQL/Exception.h"
#include "Database/PostgreSQL/Functions.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"

#include "libpq-fe.h"

#include <sstream>
#include <vector>

using namespace Database::PostgreSQL;


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection ( const Info &info ) : BaseClass(),
  _info ( info ),
  _connection ( 0x0 ),
  _cancel ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Connection::RefPtr Connection::connect ( 
  const std::string &database, 
  const std::string &user, 
  const std::string &password )
{
  Connection::Info info;
  info["dbname"] = database;
  info["user"] = user;
  info["password"] = password;
  return Connection::connect ( info );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Connection::RefPtr Connection::connect ( const Info &info )
{
  Connection::RefPtr c ( new Connection ( info ) );
  c->_connect();
  return c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::~Connection()
{
  Usul::Functions::noThrow ( boost::bind ( &Connection::_destroy, this ), "2287996467" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to the database.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_connect()
{
  Guard guard ( _connection.mutex() );

  Info info ( _info );

  if ( 0x0 != _connection )
  {
    throw Exception ( "1127589338", "Connection not null" );
  }

  if ( true == info.empty() )
  {
    throw Exception ( "2122909937", "Connection info empty", 
      Exception::Ptr(), "", info["database"], info["user"] );
  }

  // Make the connection string.
  std::ostringstream s;
  for ( Info::const_iterator i = info.begin(); i != info.end(); ++i )
  {
    s << i->first << '=' << i->second << ' ';
  }

  // Get the connection string.
  std::string cs ( s.str() );

  // Trim off the last empty space.
  if ( false == cs.empty() )
  {
    cs.erase ( cs.size() - 1 );
  }

  // Make the connection.
  const char *ccs ( cs.c_str() );
  _connection = ::PQconnectdb ( ccs );

  // Check for errors.
  Functions::check ( "4002480116", _connection, "Failed to connect", 
    info["database"], info["user"] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_destroy()
{
  // Done with this.
  _info.clear();

  // Make copies of local variables and clear them.
  PGconn *connection ( _connection );
  PGcancel *cancel ( _cancel );
  _connection = 0x0;
  _cancel = 0x0;

  // Free the cancel info.
  if ( 0x0 != cancel )
  {
    ::PQfreeCancel ( cancel );
  }

  // Free the connection.
  if ( 0x0 != connection )
  {
    ::PQfinish ( connection );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result::RefPtr Connection::execute ( const std::string &sql )
{
  // Only one thread at a time in here.
  Guard guard ( _connection.mutex() );

  // No empty strings.
  if ( true == sql.empty() )
  {
    throw Exception ( "4212500198", "Empty SQL string" );
  }

  Info info ( _info );

  // Check state.
  ::PGconn *connection ( _connection );
  if ( 0x0 == connection )
  {
    throw Exception ( "2288932197", "Null connection pointer", 
      Exception::Ptr(), sql, info["database"], info["user"] );
  }

  // Initialize the result in this scope.
  PGresult *result ( 0x0 );

  // Need local scope to ensure that the cancellation object is reset 
  // before the connection guard (at the top) is released.
  {
    // Always reset the cancellation object.
    Usul::Scope::Caller::RefPtr resetCancel ( Usul::Scope::makeCaller ( 
      boost::bind ( &Connection::_setCancel, this, 
      static_cast < PGcancel * > ( 0x0 ) ) ) );
  
    // Set the cancellation object now because the user can access 
    // from another thread to cancel a running request.
    this->_setCancel ( ::PQgetCancel ( connection ) );
  
    // Execute the statement. This blocks until it finishes.
    result = ::PQexec ( connection, sql.c_str() );

    // Check results.
    Functions::check ( "4016476262", result, sql, 
      "Failed to execute statement", info["database"], info["user"] );
  }

  // Return the new result.
  return new Result ( result, sql );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel a running statement. 
//  Return an error string if the cancellation failed. 
//  This function has no effect if there is no running statement. 
//  This function is meant to be safe to call from any thread.
//
///////////////////////////////////////////////////////////////////////////////

std::string Connection::cancelRunningStatement()
{
  std::string result;
  PGcancel *cancel ( _cancel );
  if ( 0x0 != cancel )
  {
    std::vector<char> buffer ( 512, '\0' );
    if ( 0 == ::PQcancel ( cancel, &buffer[0], buffer.size() - 1 ) )
    {
      result.assign ( &buffer[0] );
    }
  }
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cancellation object.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_setCancel ( PGcancel *cancel )
{
  _cancel = cancel;
}
