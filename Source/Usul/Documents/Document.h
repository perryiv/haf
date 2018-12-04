
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base document class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DOCUMENTS_BASE_DOCUMENT_CLASS_H_
#define _USUL_DOCUMENTS_BASE_DOCUMENT_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Atomic/Bool.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Interfaces/IClearObject.h"
#include "Usul/Interfaces/IDestroyObject.h"
#include "Usul/Interfaces/IDocumentGet.h"
#include "Usul/Interfaces/IModifiedListener.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Interfaces/IReferenceCount.h"

#include "boost/noncopyable.hpp"

#include <vector>


namespace Usul {
namespace Documents {


class USUL_EXPORT Document : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IClearObject,
  public Usul::Interfaces::IDestroyObject,
  public Usul::Interfaces::IDocumentGet,
  public Usul::Interfaces::IModifiedSubject,
  public Usul::Interfaces::IReferenceCount
{
public:

  // Typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector < IUnknown::RefPtr > Views;
  typedef Usul::Interfaces::IModifiedListener IModifiedListener;
  typedef std::vector < IModifiedListener::RefPtr > ModifiedListeners;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( Document );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Usul::Interfaces::IClearObject
  virtual void                    clearObject();

  // Usul::Interfaces::IDestroyObject
  virtual void                    destroyObject();

  // Usul::Interfaces::IDocumentGet
  virtual const Document *        documentGet() const;
  virtual Document *              documentGet();

  // Usul::Interfaces::IReferenceCount
  virtual unsigned long           getReferenceCount() const;

  // Does the document have unsaved changes?
  bool                            isModified() const;

  // Set the modified state.
  void                            modified ( bool );

  // Usul::Interfaces::IModifiedSubject
  virtual void                    modifiedListenerAdd    ( IUnknown::RefPtr );
  virtual void                    modifiedListenerRemove ( IUnknown::RefPtr );

  // Save this document. Default implementation simply returns false.
  virtual bool                    save();

  // Return a string suitable for a GUI window's title bar.
  virtual std::string             title() const;

  // Manage views.
  void                            viewAdd ( IUnknown::RefPtr );
  unsigned int                    viewCount() const;
  void                            viewRemove ( IUnknown::RefPtr );
  void                            viewsForward();

protected:

  // Constructor
  Document();

  // Use reference counting.
  virtual ~Document();

  void                            _notifyModifiedListeners();

private:

  void                            _destroy();

  Usul::Atomic::Bool _modified;
  Usul::Atomic::Container < Views > _views;
  Usul::Atomic::Container < ModifiedListeners > _modifiedListeners;
};


} // namespace Documents
} // namespace Usul


#endif // _USUL_DOCUMENTS_BASE_DOCUMENT_CLASS_H_
