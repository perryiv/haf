
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for finding files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_FIND_H_
#define _USUL_FILE_FIND_H_

#include "boost/filesystem.hpp"

#include <string>


namespace Usul {
namespace File {
namespace Find {


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for finding files with given extension.
//
///////////////////////////////////////////////////////////////////////////////

struct HasExtension
{
  HasExtension ( const std::string &ext ) : _ext ( ext ){}
  template < class T > bool operator () ( const T &file ) const
  {
    return ( _ext == boost::filesystem::extension ( file ) );
  }
private:
  const std::string _ext;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Find all files that match the given predicate.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Pred,
  class Files
>
inline void find ( 
  const boost::filesystem::path &dir, 
  Pred pred, 
  bool recursive, 
  Files &files )
{
  typedef boost::filesystem::directory_iterator Itr;

  if ( true == boost::filesystem::exists ( dir ) )
  {
    Itr end;
    for ( Itr i ( dir ); i != end; ++i )
    {
      if ( boost::filesystem::is_directory ( i->status() ) )
      {
        if ( true == recursive )
        {
          Usul::File::Find::find 
            ( i->path(), pred, recursive, files );
        }
      }
      else // file
      {
        boost::filesystem::path file ( *i );
        if ( true == pred ( file ) )
        {
          files.insert ( files.end(), file.string() );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all files that match the given predicate.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Pred,
  class Files
>
inline void find ( 
  const std::string &dir, 
  Pred pred, 
  bool recursive, 
  Files &files )
{
  boost::filesystem::path path ( dir );
  Usul::File::Find::find ( path, pred, recursive, files );
}


} // namespace Find
} // namespace File
} // namespace Usul


#endif // _USUL_FILE_FIND_H_
