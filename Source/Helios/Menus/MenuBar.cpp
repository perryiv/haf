
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
//  Class that represents a menubar.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Menus/MenuBar.h"

#include "Usul/Functions/NoThrow.h"

#include <algorithm>

using namespace Helios::Menus;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar() : BaseClass ( std::string() ), 
  _menus(),
  _dirty ( true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::~MenuBar()
{
  USUL_TRY_BLOCK
  {
    _menus.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "3542068475" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::Position MenuBar::find ( Menu::RefPtr menu ) const
{
  Guard guard ( this );
  MenuList::const_iterator i ( std::find ( _menus.begin(), _menus.end(), menu ) );
  return ( ( _menus.end() == i ) ? -1 : std::distance ( _menus.begin(), i ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper predicate for finding the menu with given text.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct HasText
  {
    HasText ( const std::string &text ) : _text ( text ){}
    bool operator () ( const Helios::Menus::Menu::RefPtr &menu )
    {
      return ( ( true == menu.valid() ) ? ( menu->text() == _text ) : false );
    }
  private:
    const std::string _text;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::Position MenuBar::find ( const std::string &text ) const
{
  Guard guard ( this );
  MenuList::const_iterator i ( std::find_if 
    ( _menus.begin(), _menus.end(), Helper::HasText ( text ) ) );
  return ( ( _menus.end() == i ) ? -1 : std::distance ( _menus.begin(), i ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::remove ( Position i )
{
  Guard guard ( this );

  if ( ( i < 0 ) || ( i > _menus.size() ) )
    return;

  _menus.erase ( _menus.begin() + i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::insert ( Position i, Menu::RefPtr menu )
{
  Guard guard ( this );

  if ( false == menu.valid() )
    return;

  if ( ( -1 == i ) || ( i > _menus.size() ) )
    i = _menus.size();

  _menus.insert ( _menus.begin() + i, menu );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find or create menu.
//
///////////////////////////////////////////////////////////////////////////////

Menu::RefPtr MenuBar::menu ( const std::string &text, Position p )
{
  Guard guard ( this );

  // Look for it.
  Menu::RefPtr m ( this->at ( this->find ( text ) ) );
  if ( true == m.valid() )
    return m;

  // Make a new one and insert it.
  m = Menu::RefPtr ( new Menu ( text ) );
  this->insert ( p, m );
  return m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the menu or null.
//
///////////////////////////////////////////////////////////////////////////////

Menu::RefPtr MenuBar::at ( Position p )
{
  Guard guard ( this );
  return ( ( ( p >= 0 ) && ( p < _menus.size() ) ) ? _menus.at ( p ) : Menu::RefPtr ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::dirty ( bool state )
{
  Guard guard ( this );
  _dirty = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool MenuBar::isDirty() const
{
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the items.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::visitItems ( Visitor &v )
{
  Guard guard ( this );
  for ( MenuList::iterator i = _menus.begin(); i != _menus.end(); ++i )
  {
    Menu::RefPtr m ( *i );
    if ( true == m.valid() )
    {
      m->accept ( v );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the menu empty?
//
///////////////////////////////////////////////////////////////////////////////

bool MenuBar::empty() const
{
  Guard guard ( this );
  return _menus.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove empty menus.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::purge()
{
  Guard guard ( this );

  MenuList menus;
  menus.reserve ( _menus.size() );

  for ( MenuList::iterator i = _menus.begin(); i != _menus.end(); ++i )
  {
    Menu::RefPtr m ( *i );
    if ( true == m.valid() )
    {
      m->purge();
      if ( false == m->empty() )
      {
        menus.push_back ( m );
      }
    }
  }

  _menus = menus;
}
