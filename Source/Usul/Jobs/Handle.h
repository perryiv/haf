
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job-handle class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_JOBS_HANDLE_H_
#define _USUL_JOBS_HANDLE_H_

#include "Usul/Config/Config.h"

#include <utility>


namespace Usul {
namespace Jobs {


///////////////////////////////////////////////////////////////////////////////
//
//  Job-handle class.
//
///////////////////////////////////////////////////////////////////////////////

struct Handle
{
  typedef long Priority;
  typedef unsigned long ID;
  typedef std::pair < Priority, ID > Data;

  Handle() : _data ( 0, 0 )
  {
  }

  Handle ( ID id, Priority p ) : _data ( id, p )
  {
  }

  Handle ( const Handle &h ) : _data ( h._data )
  {
  }

  Handle &operator = ( const Handle &h )
  {
    _data = h._data;
    return *this;
  }

  ID id() const
  {
    return _data.second;
  }

  Priority priority() const
  {
    return _data.first;
  }

  const Data &data() const
  {
    return _data;
  }

private:

  Data _data;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Operators.
//
///////////////////////////////////////////////////////////////////////////////

inline bool operator == ( const Handle &left, const Handle &right )
{
  return ( left.data() == right.data() );
}
inline bool operator != ( const Handle &left, const Handle &right )
{
  return ( left.data() != right.data() );
}
inline bool operator < ( const Handle &left, const Handle &right )
{
  return ( left.data() < right.data() );
}
inline bool operator > ( const Handle &left, const Handle &right )
{
  return ( left.data() > right.data() );
}
inline bool operator <= ( const Handle &left, const Handle &right )
{
  return ( left.data() <= right.data() );
}
inline bool operator >= ( const Handle &left, const Handle &right )
{
  return ( left.data() >= right.data() );
}


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_HANDLE_H_
