
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates database environment.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_ORACLE_ENVIRONMENT_H_
#define _DATABASE_ORACLE_ENVIRONMENT_H_

#include "Database/Oracle/Export.h"

namespace oracle { namespace occi { class Environment; } }


namespace Database {
namespace Oracle {


class DATABASE_ORACLE_EXPORT Environment
{
public:

  static void                     destroy();

  static Environment &            instance();

  oracle::occi::Environment *     env();

protected:

  Environment();
  ~Environment();

private:

  // Can not copy or assign.
  Environment ( const Environment & );
  Environment &operator = ( const Environment & );

  void                            _destroy();

  oracle::occi::Environment *_env;
};


} // namespace Oracle
} // namespace Database


#endif // _DATABASE_ORACLE_ENVIRONMENT_H_
