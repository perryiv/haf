
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Exception class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_POSTGRESQL_EXCEPTION_H_
#define _DATABASE_POSTGRESQL_EXCEPTION_H_

#include "Database/PostgreSQL/Export.h"

#include "Usul/Exceptions/Exceptions.h"

#include <string>


namespace Database {
namespace PostgreSQL {


class DATABASE_POSTGRESQL_EXPORT Exception : 
  public Usul::Exceptions::Error
{
public:

  typedef Usul::Exceptions::Error BaseClass;
  typedef BaseClass::Ptr Ptr;

  Exception ( const std::string &id,
              const std::string &message,
              Ptr nested = Ptr(),
              const std::string &sql = std::string(),
              const std::string &database = std::string(), 
              const std::string &user = std::string() );

  Exception ( const std::string &id,
              const std::string &message,
              const BaseClass &nested,
              const std::string &sql = std::string(),
              const std::string &database = std::string(), 
              const std::string &user = std::string() );

  Exception ( const std::string &id,
              const std::string &message,
              const std::exception &nested,
              const std::string &sql = std::string(),
              const std::string &database = std::string(), 
              const std::string &user = std::string() );

  Exception ( const Exception & );

  virtual ~Exception() throw();

  virtual Ptr             clone() const;

  const std::string &     database() const;

  const std::string &     sql() const;

  const std::string &     user() const;

private:

  const std::string _database;
  const std::string _user;
  const std::string _sql;
};


} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_EXCEPTION_H_
