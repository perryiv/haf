
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to a file dialog.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_FILE_DIALOG_H_
#define _QT_TOOLS_FILE_DIALOG_H_

#include "Tools/Qt/Export.h"

#include "Usul/Interfaces/IDocumentFilters.h"

#include <vector>

class QWidget;


namespace Tools {
namespace Qt {


struct QT_TOOLS_EXPORT FileDialog
{
  // Typedefs.
  typedef Usul::Interfaces::IDocumentFilters::Filter Filter;
  typedef Usul::Interfaces::IDocumentFilters::Filters Filters;
  typedef std::vector < std::string > FileNames;
  
  // Get the file(s) to open.
  static std::string    getOpenFileName  ( QWidget* parent, const std::string &title, const Filters & );
  static void           getOpenFileNames ( QWidget* parent, const std::string &title, const Filters &, FileNames & );
  
  // Get the file to save to.
  static std::string    getSaveFileName  ( QWidget* parent, const std::string &title, const Filters & );
  
protected:

  static std::string   _formatFilters ( const Filters &filters );
  
  static std::string   _lastFileDialogDir ( const std::string &title );
  static void          _lastFileDialogDir ( const std::string &title, const std::string &dir );
  static std::string   _lastFileDialogFilter ( const std::string &title );
  static void          _lastFileDialogFilter ( const std::string &title, const std::string &filter );
};
  

} // namespace Qt
} // namespace Tools


#endif // _QT_TOOLS_FILE_DIALOG_H_
