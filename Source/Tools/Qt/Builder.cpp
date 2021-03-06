
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for building an interface from XML.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tools/Qt/Builder.h"

#include "Tree/Interfaces/IReadXML.h"

#include "Usul/Plugins/Manager.h"

#include "QtCore/QFile"
#include "QtGui/QWidget"
#include "QtUiTools/QUiLoader"

using namespace Tools::Qt;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Builder::Builder ( const std::string &resource, QWidget *parent ) : 
  _threadCheck(),
  _resource ( resource ),
  _widget ( 0x0 ),
  _parent ( parent )
{
  this->_build();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Builder::~Builder()
{
  if ( 0x0 != _widget )
  {
    delete _widget;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the widget and release control.
//
///////////////////////////////////////////////////////////////////////////////

QWidget *Builder::release()
{
  _threadCheck.throwIfDifferentThread();
  QWidget *w (_widget );
  _widget = 0x0;
  return w;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the widget.
//
///////////////////////////////////////////////////////////////////////////////

void Builder::_build()
{
  _threadCheck.throwIfDifferentThread();

  _widget = 0x0;

  QUiLoader loader;
  QFile file ( _resource.c_str() );
  file.open ( QFile::ReadOnly );
  if ( false == file.isOpen() )
    return;

  _widget = loader.load ( &file, _parent );
}
