
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

#ifndef _DATABASE_POSTGRESQL_CONNECTION_H_
#define _DATABASE_POSTGRESQL_CONNECTION_H_

#include "Database/PostgreSQL/Result.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"

#include <map>
#include <string>

struct pg_conn;
struct pg_cancel;
typedef pg_conn PGconn;
typedef pg_cancel PGcancel;


namespace Database {
namespace PostgreSQL {


class DATABASE_POSTGRESQL_EXPORT Connection : 
  public Usul::Base::Referenced,
  boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;
  USUL_REFERENCED_CLASS ( Connection );
  typedef std::map < std::string, std::string > Info;

  // Factory functions.
  static RefPtr           connect ( const std::string &database, 
                                    const std::string &user, 
                                    const std::string &password );
  static RefPtr           connect ( const Info & );

  // Cancel a running statement. This has no effect if there is no running 
  // statement. This is meant to be safe to call from any thread. If the 
  // cancellation fails then the returned string will contain the error.
  std::string             cancelRunningStatement();

  // Execute the sql string and return the result, which may be empty.
  Result::RefPtr          execute ( const std::string & );

protected:

  friend class Result;

  // Constructor
  Connection ( const Info & );

  // Destructor
  virtual ~Connection();

  void                    _connect();

  void                    _setCancel ( PGcancel *cancel );

private:

  void                    _destroy();

  Usul::Atomic::Container < Info > _info;
  Usul::Atomic::Object < PGconn * > _connection;
  Usul::Atomic::Object < PGcancel * > _cancel;
};


} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_CONNECTION_H_
