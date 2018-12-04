
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

#ifndef _DATABASE_POSTGRESQL_FUNCTIONS_H_
#define _DATABASE_POSTGRESQL_FUNCTIONS_H_

#include <string>


struct pg_conn;
struct pg_result;
typedef pg_conn PGconn;
typedef pg_result PGresult;


namespace Database {
namespace PostgreSQL {
namespace Functions {


void check ( const std::string &id,
             PGconn *c,
             const std::string &message = std::string(),
             const std::string &database = std::string(), 
             const std::string &user = std::string() );

void check ( const std::string &id,
             PGresult *r,
             const std::string &sql = std::string(),
             const std::string &message = std::string(),
             const std::string &database = std::string(), 
             const std::string &user = std::string() );

bool isSelectStatement ( const std::string &sql );

} // namespace Functions
} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_FUNCTIONS_H_
