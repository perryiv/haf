
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Conversion functions for use with Xercesc.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_CONVERT_H_
#define _TREE_CONVERT_H_

#include "Usul/Config/Config.h"
#include "Usul/Convert/Generic.h"

#include "xercesc/util/XMLString.hpp"

#include "boost/scoped_array.hpp"

#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  The string type to convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Tree 
{
  typedef std::basic_string < XMLCh > XercesString;
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for Xerces strings.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < Tree::XercesString, std::string >
    {
      typedef Tree::XercesString XS;
      typedef Type < XS, std::string > ThisType;
      static void convert ( const XS &from, std::string &to )
      {
        if ( false == from.empty() )
        {
          boost::scoped_array < char > ptr ( xercesc::XMLString::transcode ( from.c_str() ) );
          to = ( ( 0x0 == ptr.get() ) ? std::string() : std::string ( ptr.get() ) );
        }
      }
      static std::string convert ( const XS &from )
      {
        std::string to;
        ThisType::convert ( from, to );
        return to;
      }
    };
    template <> struct Type < std::string, Tree::XercesString >
    {
      typedef Tree::XercesString XS;
      typedef Type < std::string, XS > ThisType;
      static void convert ( const std::string &from, XS &to )
      {
        if ( false == from.empty() )
        {
          boost::scoped_array < XMLCh > ptr ( xercesc::XMLString::transcode ( from.c_str() ) );
          to = ( ( 0x0 == ptr.get() ) ? XS() : XS ( ptr.get() ) );
        }
      }
      static XS convert ( const std::string &from )
      {
        XS to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


#endif // _TREE_CONVERT_H_
