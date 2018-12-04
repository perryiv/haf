
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generic input operator for copying the results. This is in a separate 
//  header file because it is optional, and not always desired.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_SQLITE_GENERIC_INPUT_FROM_RESULTS_H_
#define _DATABASE_SQLITE_GENERIC_INPUT_FROM_RESULTS_H_

#include "Database/SQLite/Result.h"

#include "Usul/Strings/Format.h"
#include "Usul/MPL/SameType.h"

#include <stdexcept>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition of input operator intentionally not defined.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > Database::SQLite::Result &operator >> ( Database::SQLite::Result &result, T &t );


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining input operator for copying the results to vector type.
//
///////////////////////////////////////////////////////////////////////////////

#define DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR(vector_type) \
Database::SQLite::Result &operator >> ( Database::SQLite::Result &result, vector_type &v ) \
{ \
  typedef Database::SQLite::Blob Blob; \
  typedef vector_type VectorType; \
  typedef VectorType::value_type ValueType; \
  typedef VectorType::size_type SizeType; \
\
  USUL_ASSERT_SAME_TYPE ( SizeType, Blob::size_type ); \
  USUL_ASSERT_SAME_TYPE ( unsigned char, Blob::value_type ); \
\
  v.clear(); \
\
  Blob blob; \
  result >> blob; \
\
  if ( 0 != ( blob.size() % sizeof ( ValueType ) ) ) \
  { \
    throw std::runtime_error ( Usul::Strings::format \
      ( "Error 3281594059: Blob size is not evenly divisible by ", sizeof ( ValueType ) ) ); \
  } \
\
  const ValueType *temp ( reinterpret_cast < ValueType * > ( &blob[0] ) ); \
  const SizeType size ( blob.size() / sizeof ( ValueType ) ); \
\
  v.assign ( temp, temp + size ); \
\
  return result; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Define input operators for some common vector types.
//
///////////////////////////////////////////////////////////////////////////////

DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < unsigned short > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < unsigned int   > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < unsigned long  > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < char   > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < short  > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < int    > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < long   > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < double > );
DATABASE_SQLITE_DEFINE_VECTOR_INPUT_OPERATOR ( std::vector < float  > );


#endif // _DATABASE_SQLITE_GENERIC_INPUT_FROM_RESULTS_H_
