
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry database class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_DATABASE_CLASS_H_
#define _USUL_REGISTRY_DATABASE_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Atomic/Object.h"
#include "Usul/Registry/Node.h"
#include "Usul/Threads/Mutex.h"

#include "boost/noncopyable.hpp"


namespace Usul {
namespace Registry {


class USUL_EXPORT Database : public boost::noncopyable
{
public:

  // Typedefs.
  typedef Node::Path Path;

  // Constructor and destructor.
  Database();
  ~Database();

  // Accept the visitor.
  void                            accept ( Visitor * );

  // Convert the given string to a legal tag.
  std::string                     convertToTag ( const std::string & ) const;

  // Clear the database.
  void                            clear();

  // Destroy the singleton.
  static void                     destroy ( bool persistent = true );

  // Return the either the persistent or non-persistent singleton instance.
  static Database &               instance ( bool persistent = true );

  // Operator to return the child with the name or path. Creates child nodes as needed.
  Node &                          operator [] ( const std::string &name );
  Node &                          operator [] ( const Path &path );

  // Get the root.
  Node::RefPtr                    root();
  const Node::RefPtr              root() const;

private:

  static Database *_persistent;
  static Database *_program;
  Usul::Atomic::Object<Node::RefPtr> _root;
};


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_DATABASE_CLASS_H_
