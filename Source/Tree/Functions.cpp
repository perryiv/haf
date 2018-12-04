
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Tree/Functions.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/scoped_array.hpp"

#include <sstream>

using namespace Tree;


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there are characters other than white spaces.
//
///////////////////////////////////////////////////////////////////////////////

bool Functions::hasContent ( const std::string &s )
{
  for ( std::string::const_iterator i = s.begin(); i != s.end(); ++i )
  {
    const char c ( *i );
    if ( 0 == ::isspace ( c ) )
    {
      return true;
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace illegal xml characters.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::replaceInvalidXML ( const std::string &v )
{
  std::string s ( v );

  // Replace & first to ensure proper conversion.
  boost::algorithm::replace_all ( s, "&", "&amp;" );
  boost::algorithm::replace_all ( s, "<", "&lt;" );
  boost::algorithm::replace_all ( s, ">", "&gt;" );
  boost::algorithm::replace_all ( s, "'", "&apos;" );
  boost::algorithm::replace_all ( s, "©", "&copy;" );
  boost::algorithm::replace_all ( s, "\"", "&quot;" );

  return s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore illegal xml characters.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::restoreInvalidXML ( const std::string &v )
{
  std::string s ( v );

  // Replace & last to ensure proper conversion.
  boost::algorithm::replace_all ( s, "&lt;", "<" );
  boost::algorithm::replace_all ( s, "&gt;", ">" );
  boost::algorithm::replace_all ( s, "&apos;", "'" );
  boost::algorithm::replace_all ( s, "&copy;", "©" );
  boost::algorithm::replace_all ( s, "&quot;", "\"" );
  boost::algorithm::replace_all ( s, "&amp;", "&" );

  return s;
}
