
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macros for defining unique names.
//  See: http://www.ddj.com/showArticle.jhtml?documentID=cuj0003getov&pgno=2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PRE_PROCESS_UNIQUE_NAME_H_
#define _USUL_PRE_PROCESS_UNIQUE_NAME_H_

#include "Usul/Config/Config.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Plumbing
//
///////////////////////////////////////////////////////////////////////////////

#define _USUL_UNIQUE_NAME_1(the_prefix, append_me) the_prefix##append_me
#define _USUL_UNIQUE_NAME_2(the_prefix, append_me) _USUL_UNIQUE_NAME_1 (the_prefix, append_me)


///////////////////////////////////////////////////////////////////////////////
//
//  Make a unique name with the given prefix.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_PREFIXED_UNIQUE_NAME(the_prefix) _USUL_UNIQUE_NAME_2 (the_prefix, __LINE__)


///////////////////////////////////////////////////////////////////////////////
//
//  Make a unique name.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_UNIQUE_NAME USUL_PREFIXED_UNIQUE_NAME (unique_name_on_line_)


#endif // _USUL_PRE_PROCESS_UNIQUE_NAME_H_
