
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PROPERTY_EDITOR_COMPONENT_CLASS_H_
#define _PROPERTY_EDITOR_COMPONENT_CLASS_H_

#include "Helios/Plugins/PropertyEditor/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPropertyEditor.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

namespace Tools { namespace PropertyEditor { class Editor; } }

class QDockWidget;


namespace Helios {
namespace Plugins {
namespace PropertyEditor {


class PropertyEditorPlugin : 
  public Usul::Base::Object,
  public boost::noncopyable,
  public Usul::Interfaces::IActiveViewListener,
  public Usul::Interfaces::IPlugin,
  public Usul::Interfaces::IPropertyEditor
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Tools::PropertyEditor::Editor PropertyEditor;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IActiveViewListener IActiveViewListener;
  typedef Usul::Interfaces::IPropertyEditor IPropertyEditor;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( PropertyEditorPlugin );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  PropertyEditorPlugin();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Property Editor"; }
  virtual void                    pluginInitialize();
  virtual void                    pluginFinalize();

protected: 

  // Use reference counting.
  virtual ~PropertyEditorPlugin();

  // Usul::Interfaces::IActiveViewListener
  virtual void                    activeViewChanged ( IUnknown::RefPtr, IUnknown::RefPtr );

  // Usul::Interfaces::IPropertyEditor
  virtual void                    propertyAdd ( const std::string &name, const boost::any &value, IUnknown::RefPtr callback );

private:

  void                            _clear();

  void                            _destroy();

  QDockWidget *_dock;
  PropertyEditor *_editor;
  Usul::Threads::Check _threadCheck;
};


} // namespace PropertyEditor
} // namespace Plugins
} // namespace Helios


#endif // _PROPERTY_EDITOR_COMPONENT_CLASS_H_
