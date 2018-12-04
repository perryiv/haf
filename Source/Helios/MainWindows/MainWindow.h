
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
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_MAIN_WINDOW_H_
#define _HELIOS_MAIN_WINDOW_H_

#include "Helios/Export.h"
#include "Helios/Menus/MenuBar.h"
#include "Helios/Menus/ToolBar.h"
#include "Helios/Menus/Builder.h"

#include "Usul/Atomic/Container.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Jobs/Queue.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Check.h"

#include "QtGui/QMainWindow"
#include "QtGui/QMenuBar"
#include "QtGui/QToolBar"

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/tuple/tuple.hpp"

#include <iosfwd>
#include <map>
#include <string>

class QMdiSubWindow;


namespace Helios {
namespace MainWindows {


class HELIOS_EXPORT MainWindow : 
  public QMainWindow,
  public boost::noncopyable,
  public Usul::Interfaces::IActiveViewListener
{
  Q_OBJECT

public:

  typedef QMainWindow BaseClass;
  typedef Usul::Jobs::BaseJob BaseJob;
  typedef Usul::Jobs::BaseJob::Handle JobHandle;
  typedef JobHandle::Priority Priority;
  typedef boost::tuples::tuple < Menus::MenuBar::RefPtr, Menus::Builder::RefPtr > MenuBarData;
  typedef Usul::Atomic::Object < MenuBarData > AtomicMenuBar;
  typedef boost::shared_ptr < QToolBar > QToolBarPtr;
  typedef boost::tuples::tuple < Menus::ToolBar::RefPtr, QToolBarPtr, Menus::Builder::RefPtr, bool > ToolBarData;
  typedef std::map < std::string, ToolBarData > ToolBarMap;
  typedef Usul::Atomic::Container < ToolBarMap > AtomicToolBarMap;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Main window types.
  enum Type { SINGLE_DOCUMENT, MULTIPLE_DOCUMENT, UNKNOWN_WINDOW_TYPE };

  // Initiate an update of the toolbars the next time the application is idle.
  void                              dirtyToolBars() { this->_needToUpdateToolBars ( true ); }

  // Get menu and tool bars.
  Menus::MenuBar::RefPtr            getMenuBar();
  Menus::ToolBar::RefPtr            getToolBar ( const std::string &name );

  // Call this after the plugins are loaded.
  virtual void                      init();

  // Manage plugins.
  std::string                       pluginsFile() const;
  void                              pluginsLoad();
  void                              pluginsPrint ( std::ostream & ) const;

  // Return the name of the setting file.
  std::string                       settingsFile() const;

  // Show this window.
  void                              show();

  // Return object for checking threads.
  Usul::Threads::Check              threadCheck() const;

  // Return the main-window type.
  virtual Type                      type() const { return MainWindow::UNKNOWN_WINDOW_TYPE; }

protected:

  // Constructor and destructor.
  explicit MainWindow();
  virtual ~MainWindow();

  // Usul::Interfaces::IActiveViewListener
  virtual void                      activeViewChanged ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView );

  virtual void                      closeEvent ( QCloseEvent *event );
  virtual void                      _createGui ( Usul::Interfaces::IUnknown::RefPtr );

  virtual void                      _documentOpen(){}
  virtual void                      _documentOpen ( const std::string & );
  virtual void                      _documentNew();
  void                              _documentSave();

  void                              _enableIfHasPlugin ( unsigned long, QAction & ) const;

  bool                              _forwardExistingFile ( const std::string & );

  void                              _initDockableWindowMenu();

  void                              _needToUpdateToolBars ( bool state );

  void                              _restoreDockWindows();

  void                              _settingsLoad();
  void                              _settingsSave();
  virtual void                      _showChildWindow ( QMdiSubWindow * ){}

  void                              _updateDocumentSave ( QAction &action ) const;
  void                              _updateMenuBar();
  void                              _updateToolBar ( ToolBarData & );
  void                              _updateToolBars();

private slots:
 
  void                              _idleProcess();

private:

  void                              _destroy();

  Usul::Atomic::Integer<unsigned long> _refCount;
  QTimer *_idleTimer;
  AtomicMenuBar _menuBar;
  AtomicToolBarMap _toolBars;
};


} // namespace MainWindows
} // namespace Helios


#endif // _HELIOS_MAIN_WINDOW_H_
