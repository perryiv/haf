
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for managing transactions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_POSTGRESQL_TRANSACTION_CLASS_H_
#define _DATABASE_POSTGRESQL_TRANSACTION_CLASS_H_

#include "Database/PostgreSQL/Connection.h"


namespace Database {
namespace PostgreSQL {


struct Transaction
{
  Transaction ( Connection::RefPtr );
  ~Transaction();

  void commit();

private:

  Connection::RefPtr _c;
};


} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_TRANSACTION_CLASS_H_
