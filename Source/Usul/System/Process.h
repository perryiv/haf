
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Process class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_PROCESS_CLASS_H_
#define _USUL_SYSTEM_PROCESS_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Base/Object.h"

#include <iosfwd>
#include <string>


namespace Usul {
namespace System {


class USUL_EXPORT Process : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
#ifdef _MSC_VER
  typedef unsigned long ID;
#else
  typedef int ID;
#endif

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Process );

  // Constructor asks the system to start the process 
  // but does not wait for it to actually start.
  Process ( const std::string &executable, const std::string &arguments );

  // Get the current process's id.
  static ID               currentProcessId();

  // Get the system's ID for this object.
  ID                      id() const;

  // Is the process running?
  bool                    isRunning() const;

  // Return the number of process running with the given name.
  static unsigned int     count ( const std::string &name );

  // Stop the process.
  void                    stop();

  // Wait for the process to finish.
  void                    wait ( unsigned long milliseconds = 0xFFFFFFFF );

protected:

  // Use reference counting.
  virtual ~Process();

private:

  // No copying or assigning.
  Process ( const Process & );
  Process &operator = ( const Process & );

  void                    _destroy();

  const std::string _executable;
  const std::string _arguments;
  void *_data;
  bool _hasBeenStopped;
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_PROCESS_CLASS_H_
