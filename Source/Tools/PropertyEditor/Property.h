
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

#ifndef _PROPERTY_EDITOR_PROPERTY_CLASSES_H_
#define _PROPERTY_EDITOR_PROPERTY_CLASSES_H_

#include "Tools/PropertyEditor/CompileGuard.h"

#include "Usul/Threads/Check.h"

#include "QtCore/QObject.h"

#include "boost/noncopyable.hpp"


namespace Tools {
namespace PropertyEditor {


///////////////////////////////////////////////////////////////////////////////
//
//  String property.
//
///////////////////////////////////////////////////////////////////////////////

class StringProperty : 
  public QObject,
  public boost::noncopyable
{
  Q_OBJECT;

  Q_PROPERTY ( QString MyString READ myStringGet WRITE myStringSet USER true );

public:

  typedef QObject BaseClass;

  Property();
  virtual ~Property();

private:

  void              _destroy();

  Usul::Threads::Check _threadCheck;
};


} // namespace PropertyEditor
} // namespace Tools


#endif // _PROPERTY_EDITOR_PROPERTY_CLASSES_H_
