
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a dynamically loaded library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PLUGINS_LIBRARY_CLASS_H_
#define _USUL_PLUGINS_LIBRARY_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Atomic/Integer.h"
#include "Usul/Export/Export.h"
#include "Usul/Pointers/Pointers.h"

#include "boost/noncopyable.hpp"

#include <string>


namespace Usul {
namespace Plugins {


class USUL_EXPORT Library : 
  public Usul::Base::Referenced,
  public boost::noncopyable
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef unsigned long Function;
  typedef unsigned long ModuleHandle;

  // Smart pointers.
  USUL_REFERENCED_CLASS ( Library );

  // Construction.
  Library ( const std::string &path );

  // Get the library's file name.
  std::string           file() const;

  // Explicitely free the library. This will automatically happen in the destructor.
  void                  free();

  // Get a pointer to the function.
  Function              function ( const std::string &name ) const;

protected:

  // Only construct on the heap. Use reference counting.
  virtual ~Library();

private:

  void                  _destroy();

  Usul::Atomic::Integer < ModuleHandle > _module;
  const std::string _file;
};


} // namespace Plugins
} // namespace Usul


#endif // _USUL_PLUGINS_LIBRARY_CLASS_H_
