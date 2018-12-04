
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For storing the command line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_
#define _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Threads/Mutex.h"

#include <vector>
#include <string>


namespace Usul {
namespace CommandLine {


class USUL_EXPORT Arguments
{
public:

  // Typedefs.
  typedef std::vector < std::string > Strings;
  typedef Usul::Threads::Mutex Mutex;

  // Get the number of arguments.
  unsigned int                size() const;

  // Get the arguments.
  Strings                     get() const;

  // Returns empty string if index is out of range.
  std::string                 get ( unsigned int ) const;

  // Return the single instance.
  static Arguments &          instance();

  // Set the arguments.
  void                        set ( int argc, char **argv );

private:

  // Constructor and destructor.
  Arguments();
  ~Arguments();

  // No copying or assignment.
  Arguments ( const Arguments & );
  Arguments &operator = ( const Arguments & );

  Strings _args;
  mutable Usul::Threads::Mutex _mutex;
};


} // namespace Shared
} // namespace Usul


#endif // _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_
