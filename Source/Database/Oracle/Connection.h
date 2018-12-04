
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates all database connectivity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_ORACLE_CONNECTION_H_
#define _DATABASE_ORACLE_CONNECTION_H_

#include "Database/Oracle/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace oracle { namespace occi { class Connection; class Statement; } }

namespace Database { namespace Oracle { class Result; } }


namespace Database {
namespace Oracle {


class DATABASE_ORACLE_EXPORT Connection : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  USUL_REFERENCED_CLASS ( Connection );
  typedef Database::Oracle::Result Result;

  // Factory functions.
  static RefPtr           connect ( const std::string &database, 
                                    const std::string &user, 
                                    const std::string &password );

  // Cancel a running statement. This has no effect if there is no running 
  // statement. This is meant to be safe to call from any thread.
  void                    cancelRunningStatement();

  // Commit changes made since previous commit.
  void                    commit();

  // Execute the sql string and return the result, which may be empty.
  // Can also specify the number of row to fetch at a time.
  Result *                execute ( const std::string &, unsigned int numRowsToPreFetch = 0 );

  // Cancel any running queries for this connection on the server.
  void                    sendCancelRequestToServer();

  // Roll back any changes made since previous commit.
  void                    rollback();

protected:

  friend class Database::Oracle::Result;

  // Constructor
  Connection ( oracle::occi::Connection * );

  // Destructor
  virtual ~Connection();

  Result *                _execute ( const std::string &, unsigned int numRowsToPreFetch );
  Result *                _execute ( oracle::occi::Connection *, oracle::occi::Statement *, bool );

  void                    _setExecutingStatement ( oracle::occi::Statement *s );

  void                    _terminateStatement ( oracle::occi::Statement *s );

private:

  // Can not copy or assign.
  Connection ( const Connection & );
  Connection &operator = ( const Connection & );

  void                    _destroy();

  oracle::occi::Connection *_connection;
  oracle::occi::Statement *_executing;
};


} // namespace Oracle
} // namespace Database


#endif // _DATABASE_ORACLE_CONNECTION_H_
