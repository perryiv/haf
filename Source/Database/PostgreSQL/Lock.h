
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for managing a table lock.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_POSTGRESQL_TABLE_LOCK_CLASS_H_
#define _DATABASE_POSTGRESQL_TABLE_LOCK_CLASS_H_

#include <string>

namespace Database { namespace PostgreSQL { class Connection; } }


namespace Database {
namespace PostgreSQL {


struct Lock
{
  Lock ( Connection &,
	 const std::string &table, 
	 const std::string &mode );
  ~Lock();

  void commit();

private:

  Connection &_c;
  std::string _table;
  std::string _mode;
};


} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_TABLE_LOCK_CLASS_H_
