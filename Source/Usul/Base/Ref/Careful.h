
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counted base class that checks for errors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_CAREFUL_REFERENCE_COUNTED_CLASS_H_
#define _USUL_BASE_CAREFUL_REFERENCE_COUNTED_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace Base {
namespace Ref {


class USUL_EXPORT Careful
{
public:

  typedef unsigned long RefCount;

  void                    ref();
  void                    unref ( bool allowDelete = true );

  RefCount                refCount() const;

  virtual std::string     typeName() const;

protected:

  struct CountData
  {
    RefCount count;
    bool deleteMe;
    mutable Usul::Threads::Mutex mutex;
    CountData ( RefCount c ) : count ( c ), deleteMe ( false ), mutex(){}
  private:
    CountData ( const CountData &c );
    CountData &operator = ( const CountData & );
  };

  Careful();
  Careful ( const Careful &r );
  Careful &operator = ( const Careful &r );
  virtual ~Careful();

private:

  CountData _refCount;
  Usul::Types::UInt64 _objectNumber;
};


} // namespace Ref
} // namespace Base
} // namespace Usul


#endif // _USUL_BASE_CAREFUL_REFERENCE_COUNTED_CLASS_H_
