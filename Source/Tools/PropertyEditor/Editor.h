
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

#ifndef _PROPERTY_EDITOR_CLASS_H_
#define _PROPERTY_EDITOR_CLASS_H_

#include "Tools/PropertyEditor/Export.h"

#include "QtGui/QWidget.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

#include "boost/any.hpp"

#include <string>

namespace Tools { namespace PropertyEditor { class EditorWindow; } }


namespace Tools {
namespace PropertyEditor {


class QT_PROPERTY_EDITOR_EXPORT Editor : 
  public QWidget,
  public boost::noncopyable
{
public:

  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  Editor();
  virtual ~Editor();

  // Clear the properties.
  void                            clear();

  // Add the property.
  void                            propertyAdd ( const std::string &name, const boost::any &value, IUnknown::RefPtr callback );

private:

  void                            _destroy();

  Usul::Threads::Check _threadCheck;
  EditorWindow *_window;
};


} // namespace PropertyEditor
} // namespace Tools


#endif // _PROPERTY_EDITOR_CLASS_H_
