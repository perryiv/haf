
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
//  SDI main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_SDI_MAIN_WINDOW_H_
#define _HELIOS_SDI_MAIN_WINDOW_H_

#include "Helios/MainWindows/MainWindowMDI.h"


namespace Helios {
namespace MainWindows {


class HELIOS_EXPORT MainWindowSDI : public MainWindowMDI
{
public:

  // Useful typedefs.
  typedef MainWindowMDI BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindowSDI );

  // Constructor and destructor.
  MainWindowSDI();
  virtual ~MainWindowSDI();

  // Return the main-window type.
  virtual Type                      type() const { return MainWindow::SINGLE_DOCUMENT; }

protected:

  virtual void                      _createGui ( Usul::Interfaces::IUnknown::RefPtr );

  virtual void                      _documentNew();
  virtual void                      _documentOpen();
  virtual void                      _documentOpen ( const std::string & );

  virtual void                      _showChildWindow ( QMdiSubWindow * );

private:

  void                              _destroy();
};


} // namespace MainWindows
} // namespace Helios


#endif // _HELIOS_SDI_MAIN_WINDOW_H_
