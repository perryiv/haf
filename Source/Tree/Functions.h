
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_FUNCTIONS_H_
#define _TREE_FUNCTIONS_H_

#include "Tree/Export.h"

#include <string>


namespace Tree {


struct TREE_EXPORT Functions
{
  // Returns true if there are characters other than white spaces.
  static bool             hasContent ( const std::string & );

  // Replace/restore invalid xml characters.
  static std::string      replaceInvalidXML ( const std::string & );
  static std::string      restoreInvalidXML ( const std::string & );
};


} // namespace Tree


#endif // _TREE_FUNCTIONS_H_
