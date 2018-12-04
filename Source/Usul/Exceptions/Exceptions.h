
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Special exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_H_
#define _USUL_EXCEPTIONS_H_

#include "boost/shared_ptr.hpp"

#include <stdexcept>
#include <string>


namespace Usul {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
//  Cancelled operation.
//
///////////////////////////////////////////////////////////////////////////////

class Cancelled : public std::runtime_error
{
public:

  typedef std::runtime_error BaseClass;
  Cancelled() : BaseClass ( "Message 3018836323: Operation canceled by user" )
  {
  }
  Cancelled ( const std::string &message ) : BaseClass ( message )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Operation timed out.
//
///////////////////////////////////////////////////////////////////////////////

class TimedOut : public std::runtime_error
{
public:

  typedef std::runtime_error BaseClass;
  TimedOut() : BaseClass ( "Message 2578161880: Operation timed out" )
  {
  }
  TimedOut ( const std::string &message ) : BaseClass ( message )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Restart the operation.
//
///////////////////////////////////////////////////////////////////////////////

class Restart : public std::runtime_error
{
public:

  typedef std::runtime_error BaseClass;
  Restart() : BaseClass ( "Message 3784226813: Restart the operation" )
  {
  }
  Restart ( const std::string &message ) : BaseClass ( message )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Error exception with optional nested error.
//
///////////////////////////////////////////////////////////////////////////////

class Error : public std::runtime_error
{
public:

  typedef std::runtime_error BaseClass;
  typedef boost::shared_ptr<Error> Ptr;

  Error ( const std::string id, const std::string &message ) : 
    BaseClass ( message ),
    _id ( id ),
    _nested()
  {
  }

  Error ( const std::string id, const std::string &message, Ptr nested ) : 
    BaseClass ( message ),
    _id ( id ),
    _nested ( nested )
  {
  }

  Error ( const std::string id, const std::string &message, const Error &nested ) : 
    BaseClass ( message ),
    _id ( id ),
    _nested ( nested.clone() )
  {
  }

  Error ( const std::string id, const std::string &message, const std::exception &nested ) : 
    BaseClass ( message ),
    _id ( id ),
    _nested ( new Error ( "", ( ( nested.what() ) ? nested.what() : "" ) ) )
  {
  }

  Error ( const Error &e ) : 
    BaseClass ( e ), 
    _id ( e._id ), 
    _nested ( e._nested )
  {
  }

  virtual ~Error() throw()
  {
    try
    {
      _nested = Ptr();
    }
    catch ( ... )
    {
    }
  }

  virtual Ptr clone() const
  {
    return Ptr ( new Error ( *this ) );
  }

  Ptr nested() const
  {
    return _nested;
  }

  const std::string &id() const
  {
    return _id;
  }

  std::string message() const
  {
    return std::string ( ( this->what() ) ? this->what() : "" );
  }

private:

  Error &operator = ( const Error & );
  const std::string _id;
  Ptr _nested;
};


} // namespace Exceptions
} // namespace Usul


#endif // _USUL_EXCEPTIONS_H_
