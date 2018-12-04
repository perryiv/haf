
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what SCENE_GRAPH_QT_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_QT_IMPORT_EXPORT_H_
#define _SCENE_GRAPH_QT_IMPORT_EXPORT_H_

#include "Usul/Config/Config.h"

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_SCENE_GRAPH_QT
#   define SCENE_GRAPH_QT_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define SCENE_GRAPH_QT_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define SCENE_GRAPH_QT_EXPORT
#endif


#endif // _SCENE_GRAPH_QT_IMPORT_EXPORT_H_
