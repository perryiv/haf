
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generates a globally unique identifier.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/UniqueId.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#ifdef _MSC_VER
# ifdef WIN32_LEAN_AND_MEAN
#  undef WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
#else
# include <uuid/uuid.h>
# include <vector>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to always delete the string.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  struct FreeString
  {
    FreeString ( RPC_CSTR &s ) : _s ( s )
    {
    }
    ~FreeString()
    {
      if ( 0x0 != _s )
      {
        RPC_CSTR *p ( &_s );
        if ( 0x0 != p )
        {
          ::RpcStringFree ( p );
        }
      }
    }
  private:
    RPC_CSTR &_s;
  };
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new unique id.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::UniqueId::create()
{
#ifdef _MSC_VER

  // Make the uuid.
  ::UUID id;
  RPC_STATUS status ( ::UuidCreate ( &id ) );
  if ( RPC_S_OK != status )
  {
    throw Usul::Exceptions::Error ( "2788740763", Usul::Strings::format (
      "Unique ID generation failed, ", 
      Usul::System::LastError::message() ) );
  }

  // Convert it to a string.
  RPC_CSTR s ( 0x0 );
  Helper::FreeString autoDelete ( s );
  status = ::UuidToString ( &id, &s );
  if ( RPC_S_OK != status )
  {
    throw Usul::Exceptions::Error ( "2967373900", Usul::Strings::format (
      "Conversion from uuid to string failed, ", 
      Usul::System::LastError::message() ) );
  }

  // Check resultant string.
  if ( 0x0 == s )
  {
    throw Usul::Exceptions::Error 
      ( "3236178693", "String converted from uuid is null" );
  }
  if ( '\0' == s[0] )
  {
    throw Usul::Exceptions::Error 
      ( "3559496214", "String converted from uuid is empty" );
  }

  // Make the new string.
  const char *begin ( reinterpret_cast < const char * > ( s ) );
  std::string answer ( begin, begin + 34 );

  // Return final string.
  return answer;

#else

  uuid_t uid;
  uuid_generate ( uid );

  std::vector<char> s ( 64, '\0' );      
  uuid_unparse_lower ( uid, &s[0] );

  return std::string ( &s[0] );

#endif
}
