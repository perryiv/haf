
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for creating images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_IMAGE_LOADER_CLASS_H_
#define _QT_TOOLS_IMAGE_LOADER_CLASS_H_

#include "Tools/Qt/Export.h"

#include <iosfwd>
#include <string>

class QWidget;
class QAction;
class QLabel;
class QSplashScreen;
class QAbstractButton;


namespace Tools {
namespace Qt {


struct QT_TOOLS_EXPORT Image
{
  static void icon   ( const std::string &file, 
                       QAbstractButton *button,
                       const std::string &dir = std::string(),
                       std::ostream *errors = 0x0 );

  static void icon   ( const std::string &file,
                       QWidget *widget,
                       const std::string &dir = std::string(),
                       std::ostream *errors = 0x0 );

  static void icon   ( const std::string &file,
                       QAction *action,
                       const std::string &dir = std::string(),
                       std::ostream *errors = 0x0 );

  static void pixmap ( const std::string &file,
                       QSplashScreen *splash,
                       const std::string &dir = std::string(),
                       std::ostream *errors = 0x0 );

  static void pixmap ( const std::string &file,
                       QLabel *label,
                       const std::string &dir = std::string(),
                       std::ostream *errors = 0x0 );
};


} // namespace Qt
} // namespace Tools


#endif // _QT_TOOLS_IMAGE_LOADER_CLASS_H_
