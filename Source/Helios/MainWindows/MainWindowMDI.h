
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
//  MDI main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_MDI_MAIN_WINDOW_H_
#define _HELIOS_MDI_MAIN_WINDOW_H_

#include "Helios/MainWindows/MainWindow.h"

class QMdiArea;


namespace Helios {
namespace MainWindows {


class HELIOS_EXPORT MainWindowMDI : public MainWindow
{
public:

  // Useful typedefs.
  typedef MainWindow BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindowMDI );

  // Constructor and destructor.
  MainWindowMDI();
  virtual ~MainWindowMDI();

  // Return the main-window type.
  virtual Type                      type() const { return MainWindow::MULTIPLE_DOCUMENT; }

protected:

  virtual void                      closeEvent ( QCloseEvent *event );
  void                              _closeSubWindows();
  virtual void                      _createGui ( Usul::Interfaces::IUnknown::RefPtr );

  virtual void                      _documentOpen();
  virtual void                      _documentOpen ( const std::string & );

  const QMdiArea *                  _getMdiArea() const;
  QMdiArea *                        _getMdiArea();

  virtual void                      _showChildWindow ( QMdiSubWindow * );

  void                              _updateMdiMenu ( QAction &action ) const;

private:

  void                              _destroy();

  QMdiArea *_mdiArea;
};


} // namespace MainWindows
} // namespace Helios


#endif // _HELIOS_MDI_MAIN_WINDOW_H_
