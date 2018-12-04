
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Result for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_SQLITE_RESULTS_H_
#define _DATABASE_SQLITE_RESULTS_H_

#include "Database/SQLite/Export.h"
#include "Database/SQLite/Types.h"

#include "Usul/Base/Object.h"
#include "Usul/Types/Types.h"

#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;


namespace Database {
namespace SQLite {


class DATABASE_SQLITE_EXPORT Result : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  USUL_REFERENCED_CLASS ( Result );

  // Return the column name or empty string if index is out of range.
  // Valid range is [0,numColumns()-1].
  std::string             columnName ( unsigned int index ) const;

  // Finalize the statement now. No need to call for ordinary usage.
  void                    finalize();

  // Prepare the next row for data retrieval. 
  // Returns false if there are no more rows.
  bool                    prepareNextRow();

  // Get the number of columns.
  unsigned int            numColumns() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( float & );
  Result &                operator >> ( int & );
  Result &                operator >> ( unsigned int & );
  Result &                operator >> ( Usul::Types::Int64 & );
  Result &                operator >> ( Usul::Types::UInt64 & );
  Result &                operator >> ( Blob & );

protected:

  friend class Connection;

  // Constructor
  Result ( const std::string &sql, ::sqlite3 *, ::sqlite3_stmt * );

  // Use reference counting.
  virtual ~Result();

private:

  // No default construction.
  Result();

  // Can not copy or assign.
  Result ( const Result & );
  Result &operator = ( const Result & );

  void                    _destroy();

  ::sqlite3 *_database;
  ::sqlite3_stmt *_statement;
  unsigned int _currentColumn;
  std::string _sql;
};


} // namespace SQLite
} // namespace Database


#endif // _DATABASE_SQLITE_RESULTS_H_
