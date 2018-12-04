
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base binder class and supporting makers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_SQLITE_BINDER_H_
#define _DATABASE_SQLITE_BINDER_H_

#include "Database/SQLite/Export.h"
#include "Database/SQLite/Types.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

struct sqlite3_stmt;
struct sqlite3;


namespace Database {
namespace SQLite {


///////////////////////////////////////////////////////////////////////////////
//
//  Base binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct DATABASE_SQLITE_EXPORT Binder : public Usul::Base::Referenced
{
  typedef Usul::Base::Referenced BaseClass;
  USUL_DECLARE_REF_POINTERS ( Binder );

  // Overload to bind the data.
  virtual void      bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );

  // Static methods for creating binders.
  static RefPtr     makeBlobBinder ( const void *, unsigned int );
  static RefPtr     makeBlobBinder ( const Blob & );
  static RefPtr     makeTextBinder ( const std::string & );

protected:

  Binder(){}
  virtual ~Binder(){}

private:

  Binder ( const Binder & );
  Binder &operator = ( const Binder & );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Blob binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct DATABASE_SQLITE_EXPORT BlobBinder : public Binder
{
  typedef Binder BaseClass;
  USUL_REFERENCED_CLASS ( BlobBinder );
  virtual void bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );
  BlobBinder ( const void *b, unsigned int size ) : _blob ( b ), _size ( size ){}
protected:
  virtual ~BlobBinder(){}
private:
  BlobBinder ( const BlobBinder & );
  BlobBinder &operator = ( const BlobBinder & );
  const void * _blob;
  unsigned int _size;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Text binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct DATABASE_SQLITE_EXPORT TextBinder : public Binder
{
  typedef Binder BaseClass;
  USUL_REFERENCED_CLASS ( TextBinder );
  virtual void bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );
  TextBinder ( const std::string &t ) : _text ( t ){}
protected:
  virtual ~TextBinder(){}
private:
  TextBinder ( const TextBinder & );
  TextBinder &operator = ( const TextBinder & );
  const std::string &_text;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Generic binder type intentionally no defined.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct BinderTraits;


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-defined binder types. Make more as needed.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct BinderTraits < Database::SQLite::Blob >
{
  static Binder::RefPtr makeBinder ( const Database::SQLite::Blob &blob )
  {
    return Binder::makeBlobBinder ( blob );
  }
};
template <> struct BinderTraits < std::string >
{
  static Binder::RefPtr makeBinder ( const std::string &text )
  {
    return Binder::makeTextBinder ( text );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a simple object binder.
//
///////////////////////////////////////////////////////////////////////////////

#define DATABASE_SQLITE_DEFINE_SIMPLE_OBJECT_BINDER(object_type) \
namespace Database { namespace SQLite { \
template <> struct BinderTraits < object_type > \
{ \
  static Binder::RefPtr makeBinder ( const object_type &t ) \
  { \
    return Binder::makeBlobBinder ( &t, sizeof ( object_type ) ); \
  } \
}; } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a complex object binder that reports the size in bytes.
//
///////////////////////////////////////////////////////////////////////////////

#define DATABASE_SQLITE_DEFINE_COMPLEX_OBJECT_BINDER(object_type) \
namespace Database { namespace SQLite { \
template <> struct BinderTraits < object_type > \
{ \
  static Binder::RefPtr makeBinder ( const object_type &t ) \
  { \
    return Binder::makeBlobBinder ( t.getBytes(), t.getNumBytes() ); \
  } \
}; } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a vector binder.
//
///////////////////////////////////////////////////////////////////////////////

#define DATABASE_SQLITE_DEFINE_VECTOR_BINDER(vector_type) \
namespace Database { namespace SQLite { \
template <> struct BinderTraits < vector_type > \
{ \
  static Binder::RefPtr makeBinder ( const vector_type &v ) \
  { \
    typedef vector_type::value_type ValueType; \
    return Binder::makeBlobBinder ( &v[0], v.size() * sizeof ( ValueType ) ); \
  } \
}; } }


} // namespace SQLite
} // namespace Database


///////////////////////////////////////////////////////////////////////////////
//
//  Declare some common vector types.
//
///////////////////////////////////////////////////////////////////////////////

DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < unsigned short > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < unsigned int   > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < unsigned long  > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < char   > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < short  > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < int    > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < long   > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < double > );
DATABASE_SQLITE_DEFINE_VECTOR_BINDER ( std::vector < float  > );


#endif // _DATABASE_SQLITE_BINDER_H_
