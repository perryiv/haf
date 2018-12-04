
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for building an interface from XML.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_BUILDER_H_
#define _QT_TOOLS_BUILDER_H_

#include "Tools/Qt/Export.h"

#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

#include <string>

class QWidget;


namespace Tools {
namespace Qt {


struct QT_TOOLS_EXPORT Builder : public boost::noncopyable
{
  // Constructor and destructor.
  Builder ( const std::string &resource, QWidget *parent );
  ~Builder();

  // Return the widget and release control.
  QWidget *             release();

protected:

  void                  _build();

private:

  const Usul::Threads::Check _threadCheck;
  const std::string _resource;
  QWidget *_widget;
  QWidget *_parent;
};
  

} // namespace Qt
} // namespace Tools


#endif // _QT_TOOLS_FILE_DIALOG_H_
