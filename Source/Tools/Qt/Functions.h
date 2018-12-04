
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for use with Qt.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_FUNCTIONS_H_
#define _QT_TOOLS_FUNCTIONS_H_

#include "Tools/Qt/Export.h"

class QObject;


namespace Tools {
namespace Qt {


struct QT_TOOLS_EXPORT Functions
{
  // Traverse the hierarchy and delete the actions.
  static void deleteActions ( QObject *, bool traverse );

  // Traverse the hierarchy and delete the child objects.
  static void deleteChildren ( QObject *, bool traverse );

  // Delete the given object. Optionally delete all the children too.
  static void deleteObject ( QObject *, bool childrenToo );
};


} // namespace Qt
} // namespace Tools


#endif // _QT_TOOLS_FUNCTIONS_H_
