
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wraps up calls to the Curl library.
//  See http://curl.haxx.se/libcurl/c/libcurl-tutorial.html
//
//  Started with this class:
//  http://cadkit.svn.sourceforge.net/viewvc/cadkit/trunk/Usul/Network/Curl.h?revision=5408&view=markup
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_NETWORK_DOWNLOAD_H_
#define _USUL_NETWORK_DOWNLOAD_H_

#include "curl/curl.h"

#include "boost/noncopyable.hpp"

#include <stdexcept>
#include <sstream>
#include <vector>


namespace Usul {
namespace Network {


///////////////////////////////////////////////////////////////////////////////
//
//  Class to initialize and clean up the curl library.
//
/////////////////////////////////////////////////////////////////////////////

struct Life : public boost::noncopyable
{
  Life()
  {
    ::curl_global_init ( CURL_GLOBAL_ALL );
  }
  ~Life()
  {
    ::curl_global_cleanup();
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around curl handle.
//
///////////////////////////////////////////////////////////////////////////////

struct Handle : public boost::noncopyable
{
  Handle() : _curl ( ::curl_easy_init() )
  {
    if ( 0x0 == _curl )
    {
      throw std::runtime_error ( "Failed to open curl easy handle" );
    }
  }
  ~Handle()
  {
    try { this->cleanup(); }
    catch ( ... ){}
  }
  void cleanup()
  {
    ::CURL *curlHandle ( _curl );
    _curl = 0x0;
    if ( 0x0 != curlHandle )
    {
      ::curl_easy_cleanup ( curlHandle );
    }
  }
  ::CURL *handle()
  {
    return _curl;
  }
private:
  ::CURL *_curl;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Encode a url.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string encodeURL ( const std::string& url )
{
  Usul::Network::Handle h;
  char *s ( ::curl_easy_escape ( h.handle(), url.c_str(), url.size() ) );
  return ( ( 0x0 != s ) ? std::string ( s ) : url );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The download class.
//
/////////////////////////////////////////////////////////////////////////////

template
<
  class DataCallbackType, 
  class HeaderCallbackType
>
struct Download : public boost::noncopyable
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef DataCallbackType DataCallback;
  typedef HeaderCallbackType HeaderCallback;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Download ( const std::string &url, DataCallback dcb, HeaderCallback hcb ) :
    _url  ( url ),
    _dcb ( dcb ),
    _hcb ( hcb ),
    _error ( CURL_ERROR_SIZE + 1, '\0' )
  {
    if ( true == _url.empty() )
    {
      throw std::runtime_error ( "Empty URL given in Download constructor" );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Download the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  void download ( const std::string &post = std::string() )
  {
    // Make a handle.
    Usul::Network::Handle handle;

    // Set properties.
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_ERRORBUFFER, &_error[0] ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_URL, _url.c_str() ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEDATA, this ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEFUNCTION, &Download::_writeDataCB ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_HEADERDATA, this ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_HEADERFUNCTION, &Download::_writeHeaderCB ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_NOPROGRESS, true ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_NOSIGNAL, true ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_FOLLOWLOCATION, true ) );

    // Add post data if it's there.
    if ( false == post.empty() )
    {
      this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_POSTFIELDS, post.c_str() ) );
    }

    // Get the data.
    this->_check ( ::curl_easy_perform ( handle.handle() ) );
  }

private:

  ///////////////////////////////////////////////////////////////////////////
  //
  //  Check return code.
  //
  ///////////////////////////////////////////////////////////////////////////

  void _check ( CURLcode code ) const
  {
    if ( 0 != code )
    {
      const std::string error ( &_error[0] );
      std::ostringstream s;
      s << "Curl error: " << error << ", URL: " << _url;
      throw std::runtime_error ( s.str() );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  static size_t _writeDataCB ( void *buffer, size_t sizeOfOne, size_t numElements, void *userData )
  {
    Download *me ( reinterpret_cast<Download *> ( userData ) );
    return ( ( 0x0 == me ) ? 0 : me->_writeData ( buffer, sizeOfOne, numElements ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  static size_t _writeHeaderCB ( void *buffer, size_t sizeOfOne, size_t numElements, void *userData )
  {
    Download *me ( reinterpret_cast<Download *> ( userData ) );
    return ( ( 0x0 == me ) ? 0 : me->_writeHeader ( buffer, sizeOfOne, numElements ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_t _writeData ( void *buffer, size_t sizeOfOne, size_t numElements )
  {
    // Initialize.
    const size_t totalBytes ( sizeOfOne * numElements );
    const char *bytes ( reinterpret_cast<const char *> ( buffer ) );

    // Call the functor.
    _dcb ( bytes, totalBytes );

    // We handled all of it.
    return totalBytes;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_t _writeHeader ( void *buffer, size_t sizeOfOne, size_t numElements )
  {
    // Initialize.
    const size_t totalBytes ( sizeOfOne * numElements );
    const char *bytes ( reinterpret_cast<const char *> ( buffer ) );

    // Call the functor.
    _hcb ( bytes, totalBytes );

    // We handled all of it.
    return totalBytes;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string _url;
  DataCallback _dcb;
  HeaderCallback _hcb;
  std::vector<char> _error;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Default header callback.
//
///////////////////////////////////////////////////////////////////////////////

struct DefaultHeaderCallback
{
  void operator () ( const char *bytes, size_t size ) const
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for downloading.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class DataCallbackType,
  class HeaderCallbackType
>
inline void download ( const std::string &url, DataCallbackType dcb, HeaderCallbackType hcb )
{
  typedef Usul::Network::Download<DataCallbackType,HeaderCallbackType> Download;
  Download d ( url, dcb, hcb );
  d.download();
}


} // namespace Network
} // namespace Usul


#endif // _USUL_NETWORK_DOWNLOAD_H_
