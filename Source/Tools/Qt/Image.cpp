
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
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

#include "Tools/Qt/Image.h"

#include "Usul/Strings/Format.h"

#include "QtGui/QAbstractButton"
#include "QtGui/QAction"
#include "QtGui/QIcon"
#include "QtGui/QImageReader"
#include "QtGui/QLabel"
#include "QtGui/QSplashScreen"
#include "QtGui/QWidget"

#include "boost/filesystem.hpp"

using namespace Tools::Qt;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to set the icon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct SetIcon;

  template <> struct SetIcon < QAbstractButton >
  {
    static void set ( QAbstractButton *b, QIcon icon )
    {
      if ( 0x0 != b )
      {
        b->setIcon ( icon );
      }
    }
  };

  template <> struct SetIcon < QWidget >
  {
    static void set ( QWidget *w, QIcon icon )
    {
      if ( 0x0 != w )
      {
        w->setWindowIcon ( icon );
      }
    }
  };

  template <> struct SetIcon < QAction >
  {
    static void set ( QAction *a, QIcon icon )
    {
      if ( 0x0 != a )
      {
        a->setIcon ( icon );
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the icon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > void set ( const std::string &name, T *t, const std::string &dir, std::ostream *errors )
  {
    if ( 0x0 == t )
    {
      return;
    }

    boost::filesystem::path path ( name );
    if ( false == boost::filesystem::exists ( path ) )
    {
      path = dir + "/" + path.string();
      if ( false == boost::filesystem::exists ( path ) )
      {
        if ( 0x0 != errors )
        {
          const std::string message ( Usul::Strings::format (
            "Warning 4409545850: Could not find file '", name, "' or '", path.string(), "'", '\n' ) );
          *errors << message << std::flush;
        }
        return;
      }
    }
    
    // Make the reader.
    QImageReader reader ( path.string().c_str() );

    // Read the image.
    QImage image ( reader.read() );
    
    // Check to see if the image was read correctly.
    if ( ( true == image.isNull() ) && ( 0x0 != errors ) )
    {
      const std::string message ( Usul::Strings::format (
        "Warning 3786892950: Could not read file '", path.string(),
        "', Reason: ", reader.errorString().toStdString(), '\n' ) );
      *errors << message << std::flush;
      return;
    }

    // Make the icon.
    QIcon icon ( QPixmap::fromImage ( image ) );
    if ( icon.isNull() )
    {
      return;
    }

    Detail::SetIcon<T>::set ( t, icon );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QAbstractButton *button, const std::string &dir, std::ostream *errors )
{
  Detail::set ( name, button, dir, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QWidget *widget, const std::string &dir, std::ostream *errors )
{
  Detail::set ( name, widget, dir, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QAction *action, const std::string &dir, std::ostream *errors )
{
  Detail::set ( name, action, dir, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > void pixmap ( const std::string &name, T *t, const std::string &dir, std::ostream *errors )
  {
    if ( 0x0 == t )
    {
      return;
    }

    boost::filesystem::path path ( name );
    if ( false == boost::filesystem::exists ( path ) )
    {
      path = dir + "/" + path.string();
      if ( false == boost::filesystem::exists ( path ) )
      {
        return;
      }
    }

    QPixmap p;
    if ( false == p.load ( path.string().c_str() ) )
    {
      return;
    }

    // If we have an image then set it.
    if ( ( false == p.isNull() ) && ( 0x0 != t ) )
    {
      t->setPixmap ( p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

void Image::pixmap ( const std::string &name, QLabel *label, const std::string &dir, std::ostream *errors )
{
  Detail::pixmap ( name, label, dir, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

void Image::pixmap ( const std::string &name, QSplashScreen *splash, const std::string &dir, std::ostream *errors )
{
  Detail::pixmap ( name, splash, dir, errors );
}
