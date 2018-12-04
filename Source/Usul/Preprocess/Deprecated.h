
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macro to mark functions as deprecated.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREPROCESS_DEPRECATED_H_
#define _USUL_PREPROCESS_DEPRECATED_H_


#if defined _MSC_VER
// See http://msdn.microsoft.com/en-us/library/044swk7y(VS.100).aspx
# define USUL_MARK_DEPRECATED(func) __declspec(deprecated) func
# define USUL_MARK_DEPRECATED_WITH_MESSAGE(func,message) __declspec(deprecated(message)) func
#elif __GNUC__
// http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Function-Attributes.html#Function-Attributes
# define USUL_MARK_DEPRECATED(func) func __attribute__ ((deprecated))
# define USUL_MARK_DEPRECATED_WITH_MESSAGE(func,message) func __attribute__ ((deprecated))
#else
# define USUL_MARK_DEPRECATED(func)
# define USUL_MARK_DEPRECATED_WITH_MESSAGE(func,message)
#endif


#endif // _USUL_PREPROCESS_DEPRECATED_H_
