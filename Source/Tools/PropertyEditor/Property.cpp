
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Property classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tools/PropertyEditor/Property.h"

#include "Usul/Functions/NoThrow.h"

#include "boost/bind.hpp"

using namespace Tools::PropertyEditor;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Property::Property() : BaseClass(),
  _threadCheck()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Property::~Property()
{
  Usul::Functions::noThrow ( boost::bind ( &Property::_destroy, this ), "1589305599" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Property::_destroy()
{
  _threadCheck.throwIfDifferentThread();
}
