
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes that represent commands that update the action.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_COMMANDS_UPDATE_H_
#define _HELIOS_COMMANDS_UPDATE_H_

#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"

class QAction;

namespace Helios {
namespace Commands {


///////////////////////////////////////////////////////////////////////////////
//
//  Base command class.
//
///////////////////////////////////////////////////////////////////////////////

class BaseUpdate : public Usul::Base::Referenced, public boost::noncopyable
{
public:

  typedef Usul::Base::Referenced BaseClass;
  USUL_REFERENCED_CLASS ( BaseUpdate );

  virtual void execute ( QAction & )
  {
  }

protected:

  BaseUpdate()
  {
  }

  virtual ~BaseUpdate()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete command class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class Update : public BaseUpdate
{
public:

  typedef BaseUpdate BaseClass;
  USUL_REFERENCED_CLASS ( Update );

  Update ( Function f ) : _function ( f )
  {
  }

  virtual void execute ( QAction &action )
  {
    _function ( action );
  }

protected:

  virtual ~Update()
  {
  }

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for making commands.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > inline BaseUpdate::RefPtr makeUpdate ( Function fun )
{
  typedef Update < Function > UpdateType;
  return BaseUpdate::RefPtr ( new UpdateType ( fun ) );
}


} // namespace Commands
} // namespace Helios


#endif // _HELIOS_COMMANDS_UPDATE_H_
