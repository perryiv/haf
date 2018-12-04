
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
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_APPLICATION_H_
#define _HELIOS_APPLICATION_H_

#include "Helios/Export.h"

#include "Usul/Threads/Check.h"

#include "QtGui/QApplication"

#include "boost/noncopyable.hpp"

namespace Helios { namespace MainWindows { class MainWindow; } }


namespace Helios {


class HELIOS_EXPORT Application : public QApplication, public boost::noncopyable
{
public:

  // Useful typedefs.
  typedef QApplication BaseClass;
  typedef Usul::Threads::Check ThreadCheck;
  typedef MainWindows::MainWindow MainWindow;

  // Singleton interface.
  static void           init ( int &argc, char **argv );
  static Application &  instance();
  static void           destroy();

  // Get/set the main window.
  MainWindow *          mainWindow();

  // Notification of every event.
  virtual bool          notify ( QObject *, QEvent * );

  // Return object for checking threads.
  ThreadCheck           threadCheck() const { return _threadCheck; }

  // Use as stack variable to create and destroy singleton.
  struct Life
  {
    Life ( int &argc, char **argv )
    {
      Application::init ( argc, argv );
      Application::instance();
    }
    ~Life()
    {
      Application::destroy();
    }
  };

protected:

  // Constructor and destructor. QApplication takes a reference to argc.
  // Passing by value will cause a crash on Fedora 7 with gcc 4.0.
  Application ( int &argc, char **argv );
  ~Application();

  // Notification of every event.
  bool                  _notify ( QObject *, QEvent * );

  // The main-window's constructor and destructor will call this.
  friend class MainWindows::MainWindow;
  void                  _setMainWindow ( MainWindow * );

private:

  const ThreadCheck _threadCheck;
  MainWindow *_mainWindow;
};


} // namespace Helios


#endif // _HELIOS_APPLICATION_H_
