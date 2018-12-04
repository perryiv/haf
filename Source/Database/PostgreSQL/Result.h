
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates the result of a query.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_POSTGRESQL_RESULT_H_
#define _DATABASE_POSTGRESQL_RESULT_H_

#include "Database/PostgreSQL/Export.h"
#include "Database/PostgreSQL/Types.h"

#include "Usul/Base/Object.h"

#include "boost/noncopyable.hpp"

#include <string>

namespace Database { namespace PostgreSQL { class Connection; } }

struct pg_result;
typedef pg_result PGresult;


namespace Database {
namespace PostgreSQL {


class DATABASE_POSTGRESQL_EXPORT Result :
  public Usul::Base::Object,
  boost::noncopyable
{
public:

  typedef Usul::Base::Object BaseClass;
  USUL_REFERENCED_CLASS ( Result );

  // Prepare the next row for data retrieval. 
  // Returns false if there are no more rows.
  bool                    prepareNextRow();

  // Get the number of rows and columns.
  unsigned int            numColumns() const;
  unsigned int            numRows() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( int & );
  Result &                operator >> ( unsigned int & );
  Result &                operator >> ( char & );
  Result &                operator >> ( Blob & );

protected:

  friend class Connection;

  // Constructor
  Result ( PGresult *r, const std::string &sql );

  // Destructor
  virtual ~Result();

private:

  void                    _destroy();

  PGresult *_result;
  int _currentRow;
  int _currentColumn;
  const std::string _sql;
};


} // namespace PostgreSQL
} // namespace Database


#endif // _DATABASE_POSTGRESQL_RESULT_H_
