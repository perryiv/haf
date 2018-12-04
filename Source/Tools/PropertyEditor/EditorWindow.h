
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

#ifndef _PROPERTY_EDITOR_WINDOW_CLASS_H_
#define _PROPERTY_EDITOR_WINDOW_CLASS_H_

#include "Tools/PropertyEditor/CompileGuard.h"

#include "Usul/Threads/Check.h"

#include "QPropertyEditor/QPropertyEditorWidget.h"

#include "boost/noncopyable.hpp"


namespace Tools {
namespace PropertyEditor {


class EditorWindow : 
  protected QPropertyEditorWidget,
  public boost::noncopyable
{
public:

  typedef QPropertyEditorWidget BaseClass;

  EditorWindow();
  virtual ~EditorWindow();

  // Clear the properties.
  void              clear();

private:

  void              _destroy();

  Usul::Threads::Check _threadCheck;
};


} // namespace PropertyEditor
} // namespace Tools


#endif // _PROPERTY_EDITOR_WINDOW_CLASS_H_
