
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The property editor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tools/PropertyEditor/EditorWindow.h"

#include "Usul/Functions/NoThrow.h"

#include "boost/bind.hpp"

using namespace Tools::PropertyEditor;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditorWindow::EditorWindow() : BaseClass(),
  _threadCheck()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EditorWindow::~EditorWindow()
{
  Usul::Functions::noThrow ( boost::bind ( &EditorWindow::_destroy, this ), "3174794123" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void EditorWindow::_destroy()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this instance.
//
///////////////////////////////////////////////////////////////////////////////

void EditorWindow::clear()
{
  _threadCheck.throwIfDifferentThread();
  this->setObject ( 0x0 );
}
