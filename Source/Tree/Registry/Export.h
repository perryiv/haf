
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what TREE_REGISTRY_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_REGISTRY_DLL_EXPORT_H_
#define _TREE_REGISTRY_DLL_EXPORT_H_

#include "Usul/Config/Config.h"

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_TREE_REGISTRY
#   define TREE_REGISTRY_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define TREE_REGISTRY_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define TREE_REGISTRY_EXPORT
#endif

#endif // _TREE_REGISTRY_DLL_EXPORT_H_
