
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenient base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_OBJECT_CLASS_H_
#define _USUL_BASE_OBJECT_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Export/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace Usul {
namespace Base {


class USUL_EXPORT Object : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Object );

  // Get the mutex.
  Mutex &                   mutex() const;

  // Set/get the name.
  std::string               name() const;
  void                      name ( const std::string &s );

protected:

  // Construction.
  Object();

  // Construction.
  Object ( const std::string & );

  // Copy construction.
  Object ( const Object & );

  // Assignment.
  Object &operator = ( const Object & );

  // Use reference counting.
  virtual ~Object();

private:

  Usul::Atomic::Container < std::string > _name;
  mutable Mutex _mutex;
};


} // namespace Base
} // namespace Usul


#endif // _USUL_BASE_OBJECT_CLASS_H_
