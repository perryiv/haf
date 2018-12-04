
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Find algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_FIND_H_
#define _USUL_ALGORITHMS_FIND_H_

#include "boost/algorithm/string/case_conv.hpp"

#include <string>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Find algorithm that can be case-insensitive.
//
///////////////////////////////////////////////////////////////////////////////

template < class MapType >
inline typename MapType::const_iterator find
  ( bool caseSensitive, const MapType &m, const std::string &key )
{
  typedef typename MapType::mapped_type MappedType;
  typedef typename MapType::const_iterator Itr;

  // Look for the value using the given key.
  Itr i ( m.find ( key ) );

  // If we found it then we're done.
  if ( m.end() != i )
  {
    return i;
  }

  // If we're doing a case-insensitive search...
  if ( false == caseSensitive )
  {
    const std::string lowerKey ( boost::algorithm::to_lower_copy ( key ) );
    const std::string upperKey ( boost::algorithm::to_upper_copy ( key ) );

    // Loop through the map.
    for ( i = m.begin(); i != m.end(); ++i )
    {
      // Compare with lower-case key.
      const std::string lk ( boost::algorithm::to_lower_copy ( i->first ) );
      if ( lk == lowerKey )
      {
	return i;
      }

      // Compare with upper-case key.
      const std::string hk ( boost::algorithm::to_upper_copy ( i->first ) );
      if ( hk == upperKey )
      {
	return i;
      }
    }
  }

  // We didn't find the key.
  return m.end();
}


} // namespace Algorithms
} // namespace Usul


#endif // _USUL_ALGORITHMS_FIND_H_
