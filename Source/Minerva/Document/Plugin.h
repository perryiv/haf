
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_DOCUMENT_COMPONENT_CLASS_H_
#define _MINERVA_DOCUMENT_COMPONENT_CLASS_H_

#include "Minerva/Document/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IDocumentFilters.h"

#include "boost/noncopyable.hpp"


namespace Minerva {


class Plugin : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public Usul::Interfaces::IDocumentCreate,
  public Usul::Interfaces::IDocumentFilters
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IDocumentFilters IDocumentFilters;
  typedef IDocumentFilters::Filters Filters;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Plugin );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  Plugin();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Minerva Document"; }
  virtual void                    pluginInitialize(){}
  virtual void                    pluginFinalize(){}

  // Usul::Interfaces::IDocumentCreate
  virtual bool                    canCreateDocument ( const std::string &name ) const;
  virtual IUnknown::RefPtr        createDocument ( IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) );

  // Usul::Interfaces::IDocumentFilters
  virtual void                    appendDocumentFilters ( const std::string &, Filters & ) const;

protected: 

  // Use reference counting.
  virtual ~Plugin();
};


} // namespace Minerva


#endif // _MINERVA_DOCUMENT_COMPONENT_CLASS_H_
