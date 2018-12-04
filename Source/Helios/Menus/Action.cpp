
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Derived from earlier work by Adam Kubach and Perry Miller found here:
//  http://sourceforge.net/projects/cadkit/
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Action class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Menus/Action.h"

#include "Tools/Qt/Image.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Registry/Database.h"

#include "boost/filesystem.hpp"

#include <iostream>


using namespace Helios::Menus;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Action::Action ( const std::string &text, 
                 const std::string &icon, 
                 const std::string &status,
                 Command::RefPtr command, 
                 Update::RefPtr update,
                 QObject *parent ) : 
  BaseClass ( ( true == text.empty() ) ? 
                QString() : QString ( text.c_str() ), parent ),
  _command ( command ),
  _update ( update )
{
  if ( false == status.empty() )
  {
    this->setStatusTip ( status.c_str() );
  }
  if ( false == icon.empty() )
  {
    Usul::Registry::Database &reg ( Usul::Registry::Database::instance(false) );
    boost::filesystem::path dir ( Usul::CommandLine::Arguments::instance().get ( 0 ) );
    dir = dir.parent_path();
    dir = reg["icon_dir"].get ( dir.string() + "/images", false );
    Tools::Qt::Image::icon ( icon, this, dir.string(), &std::cout );
  }
  QObject::connect ( this, SIGNAL ( triggered() ), this, SLOT ( _triggered() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Action::~Action()
{
  USUL_TRY_BLOCK
  {
    this->clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3641481742" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear commands.
//
///////////////////////////////////////////////////////////////////////////////

void Action::clear()
{
  _command = Command::RefPtr ( 0x0 );
  _update  = Update::RefPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the action now.
//
///////////////////////////////////////////////////////////////////////////////

void Action::updateState()
{
  if ( true == _update.valid() )
  {
    _update->execute ( *this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when it's time to update.
//
///////////////////////////////////////////////////////////////////////////////

void Action::_updateNotify()
{
  this->updateState();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Action::_triggered()
{
  if ( true == _command.valid() )
  {
    _command->execute();
  }
  this->_updateNotify();
}
