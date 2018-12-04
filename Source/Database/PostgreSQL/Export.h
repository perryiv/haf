
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what DATABASE_POSTGRESQL_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_POSTGRESQL_EXPORT_H_
#define _DATABASE_POSTGRESQL_EXPORT_H_

#ifdef _DATABASE_POSTGRESQL_STATIC
# define DATABASE_POSTGRESQL_EXPORT
#else
# ifdef _WIN32
#  pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
#  pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
#  ifdef _COMPILING_DATABASE_POSTGRESQL
#    define DATABASE_POSTGRESQL_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
#  else
#    define DATABASE_POSTGRESQL_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
#  endif
# else // _WIN32
#  define DATABASE_POSTGRESQL_EXPORT
# endif
#endif

#endif // _DATABASE_POSTGRESQL_EXPORT_H_
