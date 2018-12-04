
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

#include "Database/PostgreSQL/Result.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Scope/Call.h"
#include "Usul/Strings/Format.h"

#include "boost/bind.hpp"

#include "libpq-fe.h"

using namespace Database::PostgreSQL;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Result::Result ( PGresult *r, const std::string &sql ) : BaseClass(),
  _result ( r ),
  _currentRow ( -1 ),
  _currentColumn ( -1 ),
  _sql ( sql )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Result::~Result()
{
  Usul::Functions::noThrow ( boost::bind ( &Result::_destroy, this ), "7136415800" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Result::_destroy()
{
  if ( 0x0 != _result )
  {
    ::PQclear ( _result );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepare the next row for data retrieval. 
//  Returns false if there are no more rows.
//
///////////////////////////////////////////////////////////////////////////////

bool Result::prepareNextRow()
{
  Guard guard ( this );

  if ( 0x0 == _result )
    return false;
    
  ++_currentRow;
  _currentColumn = 0;

  const int numRows ( static_cast < int > ( this->numRows() ) );
  return ( _currentRow < numRows );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Result::numColumns() const
{
  Guard guard ( this );

  if ( 0x0 == _result )
    return 0;

  const int num ( ::PQnfields ( _result ) );
  return ( ( num < 0 ) ? 0 : static_cast < unsigned int > ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Result::numRows() const
{
  Guard guard ( this );

  if ( 0x0 == _result )
    return 0;

  const int num ( ::PQntuples ( _result ) );
  return ( ( num < 0 ) ? 0 : static_cast < unsigned int > ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper structs to convert the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class OutputType > struct DataConverter
  {
    typedef Usul::Convert::Type < std::string, OutputType > Converter;
    static void convert ( const char *data, const unsigned int length, OutputType &value )
    {
      if ( 0x0 != data )
      {
	Converter::convert ( std::string ( data, data + length ), value );
      }
      else
      {
	value = OutputType();
      }
    }
  };
  template <> struct DataConverter < Database::PostgreSQL::Blob >
  {
    typedef Database::PostgreSQL::Blob Blob;
    static void convert ( const char *data, const unsigned int length, Blob &value )
    {
      if ( 0x0 != data )
      {
	// Convert the text array to real binary data. This will 
	// allocate memory that we have to free.
	size_t size ( 0 );
	const unsigned char *buffer ( ::PQunescapeBytea 
          ( reinterpret_cast < const unsigned char * > ( data ), &size ) );

	// Always free the buffer.
	Usul::Scope::Call freeBytes ( boost::bind 
          ( ::PQfreemem, const_cast < unsigned char * > ( buffer ) ) );

	// Copy to the value.
	value.data.assign ( buffer, buffer + size );
      }
      else
      {
	value.data.clear();
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class OutputType > 
  inline void getValue ( 
    PGresult *result, 
    const std::string &sql, 
    OutputType &value, 
    int row, 
    int &column )
  {
    if ( 0x0 == result )
    {
      throw Usul::Exceptions::Error ( "1663360101", "Null result pointer" );
    }

    // Make local copy of the column index and then increment,
    const int col ( column++ );

    // Get the data.
    const char *cell ( ::PQgetvalue ( result, row, col ) );

    // Check data.
    if ( 0x0 == cell )
    {
      throw Usul::Exceptions::Error 
	( "3672167750", Usul::Strings::format 
	  ( "Null result returned for ", "row ", row, ", column ", col ) );
    }

    // Get the length of the data. For text data we can rely on a null 
    // character at the end. However, this does not work for binary data, 
    // and this function should handle both.
    const int length ( ::PQgetlength ( result, row, col ) );

    // Handle bad length. Cell's with NULL will have zero length.
    if ( length < 0 )
    {
      throw Usul::Exceptions::Error 
	( "1511782048", Usul::Strings::format 
	  ( "Data length is ", length, " in row ", row, ", column ", col ) );
    }

    // Convert the value.
    typedef Helper::DataConverter<OutputType> Converter;
    Converter::convert ( cell, static_cast < unsigned int > ( length ), value );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( std::string &value )
{
  Guard guard ( this );
  Helper::getValue<std::string> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( double &value )
{
  Guard guard ( this );
  Helper::getValue<double> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( int &value )
{
  Guard guard ( this );
  Helper::getValue<int> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( unsigned int &value )
{
  Guard guard ( this );
  Helper::getValue<unsigned int> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( char &value )
{
  Guard guard ( this );
  Helper::getValue<char> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( Blob &value )
{
  Guard guard ( this );
  Helper::getValue<Blob> ( _result, _sql, value, _currentRow, _currentColumn );
  return *this;
}
