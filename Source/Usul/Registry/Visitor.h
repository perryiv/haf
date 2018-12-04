
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_VISITOR_CLASS_H_
#define _USUL_REGISTRY_VISITOR_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Base/Referenced.h"

#include "boost/noncopyable.hpp"

namespace Usul { namespace Registry { class Node; class Database; } }


namespace Usul {
namespace Registry {


class USUL_EXPORT Visitor : 
  public Usul::Base::Referenced,
  public boost::noncopyable
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Visitor );

  // Visit the objects.
  virtual void              visit ( Database * ) = 0;
  virtual void              visit ( Node * ) = 0;

protected:

  // Constructor.
  Visitor();

  // Use reference counting.
  virtual ~Visitor();
};


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_VISITOR_CLASS_H_
