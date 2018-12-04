
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Types for this library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_ORACLE_TYPES_H_
#define _DATABASE_ORACLE_TYPES_H_

#include "Usul/MPL/TypeWrap.h"

#include <vector>


namespace Database {
namespace Oracle {


struct Clob
{
  typedef std::vector < unsigned char > Data;
  Data data;
};

struct Blob
{
  typedef std::vector < unsigned char > Data;
  Data data;
};


} // namespace Oracle
} // namespace Database


#endif // _DATABASE_ORACLE_TYPES_H_
