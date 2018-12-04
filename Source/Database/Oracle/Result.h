
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

#ifndef _DATABASE_ORACLE_RESULT_H_
#define _DATABASE_ORACLE_RESULT_H_

#include "Database/Oracle/Export.h"
#include "Database/Oracle/Types.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace oracle { namespace occi { class Connection; class ResultSet; class Statement; } }

namespace Database { namespace Oracle { class Connection; } }


namespace Database {
namespace Oracle {


class DATABASE_ORACLE_EXPORT Result : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Result );

  // Prepare the next row for data retrieval. 
  // Returns false if there are no more rows.
  bool                    prepareNextRow();

  // Get the number of columns.
  unsigned int            numColumns() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( int & );
  Result &                operator >> ( char & );
  Result &                operator >> ( unsigned int & );
  Result &                operator >> ( unsigned char & );
  Result &                operator >> ( Clob & );
  Result &                operator >> ( Blob & );

protected:

  friend class Connection;

  // Constructor
  Result ( Connection *, oracle::occi::Statement *, oracle::occi::ResultSet * );

  // Destructor
  virtual ~Result();

private:

  // No default construction.
  Result();

  // Can not copy or assign.
  Result ( const Result & );
  Result &operator = ( const Result & );

  void                    _destroy();

  Connection *_connection;
  oracle::occi::Statement *_statement;
  oracle::occi::ResultSet *_result;
  unsigned int _currentColumn;
};


} // namespace Oracle
} // namespace Database


#endif // _DATABASE_ORACLE_RESULT_H_
