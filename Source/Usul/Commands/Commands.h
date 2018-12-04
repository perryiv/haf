
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes that represent commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMMANDS_COMMAND_H_
#define _USUL_COMMANDS_COMMAND_H_

#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"


namespace Usul {
namespace Commands {


///////////////////////////////////////////////////////////////////////////////
//
//  Base command class.
//
///////////////////////////////////////////////////////////////////////////////

class BaseCommand : public Usul::Base::Referenced, public boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;
  USUL_REFERENCED_CLASS ( BaseCommand );

  virtual void execute()
  {
  }

protected:

  BaseCommand()
  {
  }

  virtual ~BaseCommand()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete command class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class Command : public BaseCommand
{
public:

  typedef BaseCommand BaseClass;
  USUL_REFERENCED_CLASS ( Command );

  Command ( Function f ) : _function ( f )
  {
  }

  virtual void execute()
  {
    _function();
  }

protected:

  virtual ~Command()
  {
  }

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete asynchronous command class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function, class ThreadPoolPtr >
class Asynchronous : public BaseCommand
{
public:

  typedef BaseCommand BaseClass;
  USUL_REFERENCED_CLASS ( Asynchronous );

  Asynchronous ( Function fun, ThreadPoolPtr pool ) : 
    _function ( fun ),
    _pool ( pool )
  {
  }

  virtual void execute()
  {
    if ( true == _pool.valid() )
    {
      _pool->add ( _function );
    }
    else
    {
      _function();
    }
  }

protected:

  virtual ~Asynchronous()
  {
    _pool = ThreadPoolPtr ( 0x0 );
  }

  Function _function;
  ThreadPoolPtr _pool;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for making commands.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > inline BaseCommand::RefPtr makeCommand ( Function fun )
{
  typedef Command < Function > CommandType;
  return BaseCommand::RefPtr ( new CommandType ( fun ) );
}
template < class Function, class ThreadPoolPtr > inline BaseCommand::RefPtr makeCommand ( Function fun, ThreadPoolPtr pool )
{
  typedef Asynchronous < Function, ThreadPoolPtr > CommandType;
  return BaseCommand::RefPtr ( new CommandType ( fun, pool ) );
}


} // namespace Commands
} // namespace Usul


#endif // _USUL_COMMANDS_COMMAND_H_
