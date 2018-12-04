
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For reading text.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_TEXT_READER_H_
#define _USUL_IO_TEXT_READER_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"

#include <cstdio>
#include <iostream>


namespace Usul {
namespace IO {
namespace Text {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic helper reader is intentionally not defined.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > struct ReaderType
  {
    static void read ( std::istream &, T & );
  };
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Text reader class.
//
///////////////////////////////////////////////////////////////////////////////
  
struct Reader
{
  template < class T > static void read ( std::istream &in, T &t )
  {
    Helper::ReaderType<T>::read ( in, t );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read a scalar.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void read1 ( std::istream &in, T &v )
  {
    in >> v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void read2 ( std::istream &in, T &v )
  {
    typedef Usul::IO::Text::Reader Reader;
    Reader::read ( in, v[0] );
    Reader::read ( in, v[1] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void read3 ( std::istream &in, T &v )
  {
    typedef Usul::IO::Text::Reader Reader;
    Reader::read ( in, v[0] );
    Reader::read ( in, v[1] );
    Reader::read ( in, v[2] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void read4 ( std::istream &in, T &v )
  {
    typedef Usul::IO::Text::Reader Reader;
    Reader::read ( in, v[0] );
    Reader::read ( in, v[1] );
    Reader::read ( in, v[2] );
    Reader::read ( in, v[3] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void read44 ( std::istream &in, T &m )
  {
    typedef Usul::IO::Text::Reader Reader;
    Reader::read ( in, m ( 0, 0 ) );
    Reader::read ( in, m ( 0, 1 ) );
    Reader::read ( in, m ( 0, 2 ) );
    Reader::read ( in, m ( 0, 3 ) );
    Reader::read ( in, m ( 1, 0 ) );
    Reader::read ( in, m ( 1, 1 ) );
    Reader::read ( in, m ( 1, 2 ) );
    Reader::read ( in, m ( 1, 3 ) );
    Reader::read ( in, m ( 2, 0 ) );
    Reader::read ( in, m ( 2, 1 ) );
    Reader::read ( in, m ( 2, 2 ) );
    Reader::read ( in, m ( 2, 3 ) );
    Reader::read ( in, m ( 3, 0 ) );
    Reader::read ( in, m ( 3, 1 ) );
    Reader::read ( in, m ( 3, 2 ) );
    Reader::read ( in, m ( 3, 3 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class ContainerType >
  inline void readSequence ( std::istream &in, ContainerType &c )
  {
    typedef Usul::IO::Text::Reader Reader;
    typedef ContainerType Container;
    typedef typename Container::value_type ValueType;

    ValueType value;
    while ( EOF != in.peek() )
    {
      Reader::read ( in, value );
      c.insert ( c.end(), value );
    }
  }
}


} // namespace Text
} // namespace IO
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make scalar reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &v ) \
    { \
      Helper::read1 ( in, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Declare known scalars.
//
///////////////////////////////////////////////////////////////////////////////

USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( char );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( short );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( int );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( long );

USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( unsigned char );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( unsigned short );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( unsigned int );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( unsigned long );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( unsigned long long );

USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( float );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( double );
USUL_IO_TEXT_DEFINE_READER_TYPE_SCALAR ( long double );


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_2(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &v ) \
    { \
      Helper::read2 ( in, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_3(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &v ) \
    { \
      Helper::read3 ( in, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &v ) \
    { \
      Helper::read4 ( in, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make matrix reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_MATRIX_4_4(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &m ) \
    { \
      Helper::read44 ( in, m ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make sequence reader helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_READER_TYPE_SEQUENCE(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct ReaderType < the_type > \
  { \
    static void read ( std::istream &in, the_type &t ) \
    { \
      Helper::readSequence ( in, t ); \
    } \
  }; \
} } } }


#endif // _USUL_IO_TEXT_READER_H_
