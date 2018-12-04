
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

#include "Helios/Plugins/PropertyEditor/PropertyEditorPlugin.h"
#include "Helios/Application.h"
#include "Helios/MainWindows/MainWindow.h"

#include "Tools/PropertyEditor/Editor.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IPropertyEditorAccept.h"
#include "Usul/Plugins/Helper.h"

#include "QtGui/QDockWidget"

#include "boost/bind.hpp"

using namespace Helios::Plugins::PropertyEditor;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PropertyEditorPlugin, BaseClass );
USUL_PLUGIN_INITIALIZE ( PropertyEditorPlugin );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyEditorPlugin::PropertyEditorPlugin() : BaseClass(),
  _dock ( 0x0 ),
  _editor ( 0x0 ),
  _threadCheck()
{
  // We listen for changes in the active view.
  IUnknown::NoDeleteQueryPtr me ( this );
  {
    Usul::Documents::Manager::instance().activeViewListenerAdd ( me );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyEditorPlugin::~PropertyEditorPlugin()
{
  Usul::Functions::noThrow ( boost::bind ( &PropertyEditorPlugin::_destroy, this ), "3377138260" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::_destroy()
{
  // Clear most member.
  this->_clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this instance.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::_clear()
{
  // Have to delete these before the library is released.
  if ( 0x0 != _editor )
  {
    delete _editor;
    _editor = 0x0;
  }
  if ( 0x0 != _dock )
  {
    delete _dock;
    _dock = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PropertyEditorPlugin::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener * > ( this );
  case Usul::Interfaces::IPropertyEditor::IID:
    return static_cast < Usul::Interfaces::IPropertyEditor * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
///////////////////////////////////////////////////////////////////////////////

std::string PropertyEditorPlugin::pluginDescription() const
{
  return std::string ( "Property editor in a dockable window" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::pluginInitialize()
{
  Guard guard ( this );

  // Handle repeated calls.
  this->_clear();

  // Get the main window.
  MainWindows::MainWindow *mw ( Application::instance().mainWindow() );
  if ( 0x0 == mw )
    return;

  // Build the docking window.
  const std::string name ( "Property Editor" );
  _dock = new QDockWidget ( QDockWidget::tr ( name.c_str() ), mw );
  _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  // Set the object name. This is needed to save and restore dock position.
  _dock->setObjectName ( QDockWidget::tr ( name.c_str() ) );

  // Make the editor widget.
  _editor = new PropertyEditor;

  // Dock it.
  _dock->setWidget ( _editor );
  mw->addDockWidget ( Qt::LeftDockWidgetArea, _dock );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is released.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::pluginFinalize()
{
  // Stop listening for active-view changes. Cannot do this in 
  // destructor because it calls ref() and unref(). Also, if we 
  // do not do this here then the object will never get deleted.
  Usul::Documents::Manager::instance().activeViewListenerRemove
    ( IUnknown::QueryPtr ( this ) );

  // Clear this instance.
  this->_clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::activeViewChanged ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView )
{
  if ( 0x0 == _editor )
    return;

  // Clear the editor.
  _editor->clear();

  // See if the new view has properties.
  typedef Usul::Interfaces::IPropertyEditorAccept IPropertyEditorAccept;
  IPropertyEditorAccept::QueryPtr accept ( newView );
  if ( false == accept.valid() )
    return;

  // Pass this instance to the view.
  accept->propertyEditorAccept ( IPropertyEditor::QueryPtr ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the property.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyEditorPlugin::propertyAdd ( const std::string &name, 
                                         const boost::any &value,
                                         IUnknown::RefPtr callback )
{
  if ( 0x0 != _editor )
  {
    _editor->propertyAdd ( name, value, callback );
  }
}
