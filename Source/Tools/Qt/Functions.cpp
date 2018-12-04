
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for use with Qt.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tools/Qt/Functions.h"

#include "Usul/Errors/Assert.h"

#include "QtGui/QAction"
#include "QtGui/QWidget"

using namespace Tools::Qt;


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the actions.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::deleteActions ( QObject *object, bool traverse )
{
  typedef QList < QAction * > QActionList;

  if ( 0x0 == object )
    return;

  if ( true == traverse )
  {
    QObjectList c ( object->children() );
    for ( QObjectList::iterator i = c.begin(); i != c.end(); ++i )
    {
      Tools::Qt::Functions::deleteActions ( *i, traverse );
    }
  }

  QWidget *widget ( dynamic_cast < QWidget * > ( object ) );
  if ( 0x0 != widget )
  {
    QActionList a ( widget->actions() );
    for ( QActionList::iterator i = a.begin(); i != a.end(); ++i )
    {
      widget->removeAction ( *i );
    }
    USUL_ASSERT ( true == widget->actions().empty() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the children.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::deleteChildren ( QObject *object, bool traverse )
{
  if ( 0x0 == object )
    return;

  QObjectList c ( object->children() );
  for ( QObjectList::iterator i = c.begin(); i != c.end(); ++i )
  {
    Tools::Qt::Functions::deleteObject ( *i, traverse );
  }

  USUL_ASSERT ( true == object->children().empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the object and (optionally) its children.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::deleteObject ( QObject *object, bool childrenToo )
{
  if ( true == childrenToo )
  {
    Tools::Qt::Functions::deleteChildren ( object, childrenToo );
  }
  delete object;
}
