
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Manager of documents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DOCUMENTS_MANAGER_H_
#define _USUL_DOCUMENTS_MANAGER_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"
#include "Usul/Atomic/Container.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IDocumentFilters.h"

#include "boost/noncopyable.hpp"

#include <list>
#include <string>


namespace Usul {
namespace Documents {


class USUL_EXPORT Manager : public boost::noncopyable
{
public:

  // Typedefs.
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::list < IUnknown::RefPtr > DocumentSequence;
  typedef Usul::Atomic::Container < DocumentSequence > DocumentCollection;
  typedef Usul::Atomic::Object < IUnknown::RefPtr > ActiveDocument;
  typedef Usul::Atomic::Object < IUnknown::RefPtr > ActiveView;
  typedef Usul::Interfaces::IActiveDocumentListener IActiveDocumentListener;
  typedef Usul::Interfaces::IActiveViewListener IActiveViewListener;
  typedef Usul::Atomic::Container < std::list < IActiveDocumentListener::RefPtr > > ActiveDocumentListeners;
  typedef Usul::Atomic::Container < std::list < IActiveViewListener::RefPtr > > ActiveViewListeners;
  typedef Usul::Interfaces::IDocumentFilters::Filters Filters;

  // Singleton interface.
  static Manager &          instance();
  static void               destroy();
  
  // Get the active document.
  IUnknown::RefPtr          activeDocumentGet();

  // Set/get the active view.
  void                      activeViewSet ( IUnknown::RefPtr );
  template < class P > bool activeViewSetIf ( IUnknown::RefPtr newView, P predicate );
  IUnknown::RefPtr          activeViewGet();

  // Add/remove IActiveDocumentListeners.
  void                      activeDocumentListenerAdd    ( IUnknown::RefPtr );
  void                      activeDocumentListenerRemove ( IUnknown::RefPtr );

  // Add/remove IActiveViewListeners.
  void                      activeViewListenerAdd    ( IUnknown::RefPtr );
  void                      activeViewListenerRemove ( IUnknown::RefPtr );

  // Add document to internal list
  void                      addDocument ( IUnknown::RefPtr );

  // Create new document given the name. This could be a file name, 
  // database connection string, etc.
  IUnknown::RefPtr          create ( const std::string &, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) );

  // Return all file filters of the given type.
  void                      filters ( const std::string &, Filters & ) const;

  // Find the document with the given title.
  IUnknown::RefPtr          find ( const std::string &title ) const;

  // This will delete all documents that are only referenced by this object.
  void                      purge();

protected:

  void                      _notifyListenersActiveView ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView );
  void                      _notifyListenersActiveDoc  ( IUnknown::RefPtr oldDoc,  IUnknown::RefPtr newDoc  );

  void                      _setActiveDocument ( IUnknown::RefPtr );
  void                      _setActiveDocumentFromView ( IUnknown::RefPtr );

private:

  Manager();
  ~Manager();

  DocumentCollection _documents;
  ActiveDocument _activeDoc;
  ActiveView _activeView;
  ActiveDocumentListeners _activeDocListeners;
  ActiveViewListeners _activeViewListeners;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Make the new view active if the predicate is true.
//  A predicate that evaluates to true if "oldView" is the existing view is
//  std::bind2nd ( std::equal_to < IUnknown::RefPtr >(), oldView ) )
//
///////////////////////////////////////////////////////////////////////////////

template < class P > bool Manager::activeViewSetIf ( IUnknown::RefPtr newView, P pred )
{
  // Save the old view and set the new one if the predicate is true.
  IUnknown::RefPtr oldView ( 0x0 );
  {
    ActiveView::Guard guard ( _activeView.mutex() );
    IUnknown::RefPtr &active ( _activeView.getReference() );

    if ( false == pred ( active ) )
      return false;

    oldView = active;
    active = newView;
  }

  // Set the new active document.
  this->_setActiveDocumentFromView ( newView );

  // Notify listeners.
  this->_notifyListenersActiveView ( oldView, newView );

  return true;
}


} // namespace Documents
} // namespace Usul


#endif // _USUL_DOCUMENTS_MANAGER_H_
