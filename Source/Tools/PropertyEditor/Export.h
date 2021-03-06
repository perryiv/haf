
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what QT_PROPERTY_EDITOR_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_PROPERTY_EDITOR_DLL_EXPORT_H_
#define _QT_PROPERTY_EDITOR_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_QT_PROPERTY_EDITOR
#   define QT_PROPERTY_EDITOR_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define QT_PROPERTY_EDITOR_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define QT_PROPERTY_EDITOR_EXPORT
#endif

#endif // _QT_PROPERTY_EDITOR_DLL_EXPORT_H_
