
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Parent for views.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_CHILD_WINDOW_H_
#define _HELIOS_CHILD_WINDOW_H_

#include "Helios/Export.h"
#include "Helios/Menus/Button.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Interfaces/IDocumentView.h"
#include "Usul/Threads/Check.h"

#include "QtGui/QWidget"

#include "boost/noncopyable.hpp"

class QMdiSubWindow;


namespace Helios {
namespace ChildWindows {


class HELIOS_EXPORT ViewContainer : 
  public QWidget,
  public boost::noncopyable,
  public Usul::Interfaces::IDocumentView
{
  Q_OBJECT

public:

  // Useful typedefs.
  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Atomic::Object < IUnknown::RefPtr > UnknownPtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ViewContainer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor and destructor.
  ViewContainer ( IUnknown::RefPtr doc );
  virtual ~ViewContainer();

  // Usul::Interfaces::IDocumentView
  virtual void                      bringToFront();

  // Get the document.
  IUnknown::RefPtr                  document();

  // Initialize. Call immediately after constructor.
  void                              init ( QMdiSubWindow * );

  // Are we the active view?
  bool                              isActiveView() const;

  // Make this view the active one.
  void                              makeActiveView();

  // Gets called by Qt to suggest a size.
  virtual QSize                     sizeHint() const;

  // Return object for checking threads.
  Usul::Threads::Check              threadCheck() const;

protected:

  void                              _close();
  virtual void                      closeEvent ( QCloseEvent * );

  void                              _updateWindowMenuButton ( QAction &action ) const;

private:

  void                              _destroy();

  Usul::Atomic::Integer<unsigned long> _refCount;
  UnknownPtr _document;
  QMdiSubWindow *_mdiSubWindow;
  Menus::Button::RefPtr _windowMenuButton;
};


} // namespace ChildWindows
} // namespace Helios


#endif // _HELIOS_CHILD_WINDOW_H_
