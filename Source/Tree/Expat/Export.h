
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what TREE_EXPAT_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_EXPAT_DLL_EXPORT_H_
#define _TREE_EXPAT_DLL_EXPORT_H_

#include "Usul/Config/Config.h"

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_TREE_EXPAT
#   define TREE_EXPAT_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define TREE_EXPAT_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define TREE_EXPAT_EXPORT
#endif

#endif // _TREE_EXPAT_DLL_EXPORT_H_
