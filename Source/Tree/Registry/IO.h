
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that reads and writes Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_REGISTRY_IO_H_
#define _TREE_REGISTRY_IO_H_

#include "Tree/Registry/Export.h"

#include <string>

namespace Usul { namespace Registry { class Database; } }


namespace Tree {
namespace Registry {


struct TREE_REGISTRY_EXPORT IO
{
  // Read/write the registry.
  static void    read  ( const std::string &file, Usul::Registry::Database & );
  static void    write ( const std::string &file, Usul::Registry::Database & );
};


} // namespace Registry
} // namespace Tree


#endif // _TREE_REGISTRY_IO_H_
