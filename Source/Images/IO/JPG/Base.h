
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for JPG IO classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IO_JPG_BASE_CLASS_H_
#define _IMAGES_IO_JPG_BASE_CLASS_H_

#ifdef _MSC_VER
# pragma warning ( disable : 4005 )
#endif

extern "C"
{
  #include "jpeglib.h"
}

#include <string>
#include <vector>
#include <setjmp.h>


namespace Images {
namespace IO {
namespace JPG {


///////////////////////////////////////////////////////////////////////////////
//
//  Default progress functor.
//
///////////////////////////////////////////////////////////////////////////////

struct DefaultProgressFunctor
{
  void operator () ( unsigned int scanLines, unsigned int totalScanLines )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for writing a JPG file.
//
///////////////////////////////////////////////////////////////////////////////

class Base
{
protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when there is an error.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void _jpegError ( j_common_ptr cinfo )
  {
    jmp_buf *env = static_cast < jmp_buf * > ( cinfo->client_data );
    if ( 0x0 != env )
    {
      ::longjmp ( *env, 1 ); // Search code for "2159138372"
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the error message.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string getErrorMessage ( jpeg_compress_struct &cinfo )
  {
      std::string s;
      if ( 0x0 != cinfo.err )
      {
        if ( 0x0 != cinfo.err->format_message )
        {
          std::vector < char > buffer ( JMSG_LENGTH_MAX * 2, '\0' );
          (*cinfo.err->format_message) 
            ( reinterpret_cast < j_common_ptr > ( &cinfo ), &buffer[0] );
          if ( '\0' != buffer[0] )
          {
            s.assign ( &buffer[0] );
          }
        }
      }
      return ( ( s.empty() ) ? std::string ( "Unknown error" ) : s );
  }

public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the mime-type string.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string mimeType()
  {
    return std::string ( "image/jpeg" );
  }
};


} // namespace JPG
} // namespace IO
} // namespace Images


#endif // _IMAGES_IO_JPG_BASE_CLASS_H_
