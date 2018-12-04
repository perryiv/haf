
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_EXPAT_EXPAT_READER_H_
#define _TREE_EXPAT_EXPAT_READER_H_

#include "Tree/Expat/Export.h"
#include "Tree/Node.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Check.h"

#include "boost/noncopyable.hpp"

#include <iosfwd>
#include <stack>
#include <string>


namespace Tree {
namespace Expat {


class TREE_EXPAT_EXPORT Reader : public boost::noncopyable
{
public:

  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Tree::Node Node;
  typedef std::stack < Node::RefPtr > NodeStack;

  Reader();
  ~Reader();

  // Read the file.
  Node::RefPtr    read ( 
                    const std::string &file,
                    IUnknown::RefPtr caller = IUnknown::RefPtr(),
                    unsigned int bufferSize = 2048 );

  // Read the stream.
  Node::RefPtr    read ( 
                    std::istream &stream,
                    Usul::Types::UInt64 streamSize,
                    IUnknown::RefPtr caller = IUnknown::RefPtr(),
                    unsigned int bufferSize = 2048 );

private:

  void            _nodeStackClear();
  void            _nodeStackPop();
  void            _nodeStackPush ( Node::RefPtr );
  unsigned int    _nodeStackSize() const;
  Node::RefPtr    _nodeStackTop() const;

  void            _elementEnd ( const char *name );
  static void     _elementEndCB ( void *userData, const char *name );

  void            _elementStart ( const char *name, const char **atts );
  static void     _elementStartCB ( void *userData, const char *name, const char **atts );

  void            _elementValue ( const char *value, int length );
  static void     _elementValueCB ( void *userData, const char *value, int length );

  Usul::Threads::Check _threadCheck;
  NodeStack _nodeStack;
  bool _stop;
  Node::RefPtr _root;
};


} // namespace Expat
} // namespace Tree


#endif // _TREE_EXPAT_EXPAT_READER_H_
