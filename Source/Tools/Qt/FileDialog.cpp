
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

#include "Tools/Qt/FileDialog.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/Registry/Database.h"
#include "Usul/User/Directory.h"

#include "QtGui/QFileDialog"

#include "boost/filesystem.hpp"

using namespace Tools::Qt;


///////////////////////////////////////////////////////////////////////////////
//
//  Constants for the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  // Registry section.
  const std::string FILE_DIALOG    ( "file_dialog" );

  // File dialog properties.
  const std::string LAST_DIRECTORY ( "last_directory" );
  const std::string LAST_FILTER    ( "last_filter" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to load from.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::getOpenFileName ( QWidget* parent, const std::string &title, const Filters &filters )
{
  // Make the filter string.
  const std::string allFilters ( FileDialog::_formatFilters ( filters ) );

  // Get the directory.
  const std::string dir ( FileDialog::_lastFileDialogDir ( title ) );

  // Get the current filter.
  QString filter ( FileDialog::_lastFileDialogFilter ( title ).c_str() );

  // Need to use this static function to get native file dialog.
  QString answer ( QFileDialog::getOpenFileName ( parent, title.c_str(), dir.c_str(), allFilters.c_str(), &filter ) );

  // Return empty string if no file.
  if ( 0 == answer.size() )
    return std::string();

  // Get the file.
  boost::filesystem::path file ( answer.toStdString() );
  
  // Save the directory and filter.
  FileDialog::_lastFileDialogDir    ( title, file.parent_path().string() );
  FileDialog::_lastFileDialogFilter ( title, filter.toStdString() );

  // Return the answer.
  return file.string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the names of the files to load from.
//
///////////////////////////////////////////////////////////////////////////////

void FileDialog::getOpenFileNames ( QWidget* parent, const std::string &title, const Filters &filters, FileNames &files )
{
  // Initialize the answer.
  files.clear();

  // Make the filter string.
  const std::string allFilters ( FileDialog::_formatFilters ( filters ) );

  // Get the directory.
  const std::string dir ( FileDialog::_lastFileDialogDir ( title ) );

  // Get the current filter.
  QString filter ( FileDialog::_lastFileDialogFilter ( title ).c_str() );

  // Need to use this static function to get native file dialog.
  QStringList answer ( QFileDialog::getOpenFileNames ( parent, title.c_str(), dir.c_str(), allFilters.c_str(), &filter ) );

  // If we have at least one file.
  if ( false == answer.empty() )
  {
    // Get the file.
    boost::filesystem::path file ( answer.first().toStdString() );
    
    // Save the directory and filter.
    FileDialog::_lastFileDialogDir    ( title, file.parent_path().string() );
    FileDialog::_lastFileDialogFilter ( title, filter.toStdString() );
  }

  // Convert the answer.
  for( QStringList::const_iterator iter = answer.begin(); iter != answer.end(); ++iter )
  {
    files.push_back ( iter->toStdString() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to save to
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::getSaveFileName ( QWidget* parent, const std::string &title, const Filters &filters )
{
  // Make the filter string.
  const std::string allFilters ( FileDialog::_formatFilters ( filters ) );

  // Get the current filter.
  QString filter ( FileDialog::_lastFileDialogFilter ( title ).c_str() );

  // Get the directory.
  const std::string dir ( FileDialog::_lastFileDialogDir ( title ) );

  // Need to use this static function to get native file dialog.
  QString answer ( QFileDialog::getSaveFileName ( parent, title.c_str(), dir.c_str(), allFilters.c_str(), &filter ) );

  // Return empty string if no file.
  if ( 0 == answer.size() )
    return std::string();

  // Get the file.
  boost::filesystem::path file ( answer.toStdString() );
  
  // Save the directory and filter.
  FileDialog::_lastFileDialogDir    ( title, file.parent_path().string() );
  FileDialog::_lastFileDialogFilter ( title, filter.toStdString() );

  // Return the answer.
  return file.string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the given filters into a single string.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_formatFilters ( const Filters &filters ) 
{
  // Initialize the answer.
  QStringList allFilters;

  // Loop through and append each filter.
  for ( Filters::const_iterator i = filters.begin(); i != filters.end(); ++i )
  {
    const std::string filter ( i->first );
    allFilters.push_back ( filter.c_str() );
  }

  // Sort the filters.
  allFilters.sort();

  // Put a semi-colon between each filter.
  QString answer ( allFilters.join ( ";;" ) );

  // Return the answer.
  return answer.toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_lastFileDialogDir ( const std::string &title ) 
{
  USUL_TRY_BLOCK
  {
    // Copy the title and replace any white space.
    std::string key ( title );
    std::replace ( key.begin(), key.end(), ' ', '_' );

    // Get the value and return it.
    const std::string defaultDir ( Usul::User::Directory::documents() );
    const std::string dir ( Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_DIRECTORY][key].get ( defaultDir ) );
    return dir;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2637892413" );
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last directory.
//
///////////////////////////////////////////////////////////////////////////////

void FileDialog::_lastFileDialogDir ( const std::string &title, const std::string &dir )
{
  USUL_TRY_BLOCK
  {
    // Copy the title and replace any white space.
    std::string key ( title );
    std::replace ( key.begin(), key.end(), ' ', '_' );

    // Set the value and return it.
    Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_DIRECTORY][key] = dir;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1470916380" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_lastFileDialogFilter ( const std::string &title ) 
{
  USUL_TRY_BLOCK
  {
    // Copy the title and replace any white space.
    std::string key ( title );
    std::replace ( key.begin(), key.end(), ' ', '_' );

    const std::string filter ( Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_FILTER][key].get ( "" ) );
    return filter;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "4010857611" );
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

void FileDialog::_lastFileDialogFilter ( const std::string &title, const std::string &filter ) 
{
  USUL_TRY_BLOCK
  {
    // Copy the title and replace any white space.
    std::string key ( title );
    std::replace ( key.begin(), key.end(), ' ', '_' );

    Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_FILTER][key] = filter;
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1056012768" );
}
