
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
//  Note: this file is not named "Manager.cpp" because VC++ 9 could not 
//  distinguish it from the Usul/Plugins/Manager.cpp.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Documents/Manager.h"
#include "Usul/Documents/Document.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IDocumentGet.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IReferenceCount.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Plugins/Manager.h"

#include "boost/bind.hpp"

#include <algorithm>

using namespace Usul::Documents;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the singleton at startup to avoid problems with construction 
//  in multi-threaded environments.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      #ifdef USUL_AUTO_CREATE_SINGLETON_DOCUMENT_MANAGER
      Usul::Documents::Manager::instance();
      #endif
    }
    ~Init()
    {
      #ifdef USUL_AUTO_DELETE_SINGLETON_DOCUMENT_MANAGER
      Usul::Documents::Manager::destroy();
      #endif
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Documents
  {
    namespace Detail
    {
      Usul::Documents::Manager *_instance ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager& Manager::instance()
{
  if ( 0x0 == Usul::Documents::Detail::_instance )
  {
    Usul::Documents::Detail::_instance = new Manager();
  }
  return *Usul::Documents::Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
  Manager *m ( Usul::Documents::Detail::_instance );
  Usul::Documents::Detail::_instance = 0x0;
  delete m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
  _documents(),
  _activeDoc ( IUnknown::RefPtr ( 0x0 ) ),
  _activeView ( IUnknown::RefPtr ( 0x0 ) ),
  _activeDocListeners(),
  _activeViewListeners()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  USUL_TRY_BLOCK
  {
    // Purge all documents.
    this->purge();

    // Should not be an active document.
    if ( true == _activeDoc.getReference().valid() )
    {
      USUL_ASSERT ( false );
      _activeDoc = IUnknown::RefPtr ( 0x0 );
    }

    // Should not be an active view.
    if ( true == _activeView.getReference().valid() )
    {
      USUL_ASSERT ( false );
      _activeView = IUnknown::RefPtr ( 0x0 );
    }

    // Should not have any listeners.
    if ( false == _activeDocListeners.empty() )
    {
      USUL_ASSERT ( false );
      _activeDocListeners.clear();
    }

    // Should not have any listeners.
    if ( false == _activeViewListeners.empty() )
    {
      USUL_ASSERT ( false );
      _activeViewListeners.clear();
    }

    // Should not have any documents.
    if ( false == _documents.empty() )
    {
      USUL_ASSERT ( false );
      _documents.clear();
    }
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2361953971" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for finding the plugin that can create the document.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct CanCreate
  {
    CanCreate ( const std::string &name ) : _name ( name ){}
    bool operator () ( Usul::Interfaces::IUnknown::RefPtr unknown ) const
    {
      Usul::Interfaces::IDocumentCreate::QueryPtr creator ( unknown );
      return ( ( true == creator.valid() ) ? creator->canCreateDocument ( _name ) : false );
    }
  private:
    std::string _name;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new document given the name. This could be a file name, 
//  database connection string, etc.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Manager::create ( const std::string &name, IUnknown::RefPtr caller )
{
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IDocumentCreate IDocumentCreate;

  // Look for the creator. Get a shortcut to the plugin manager.
  Usul::Plugins::Manager &pm ( Usul::Plugins::Manager::instance() );

  // Note: g++ 4.3.2 on Ubuntu 4.3.2-1ubuntu12 did not like the following 
  // two lines to be combined; it could not find the right QueryPointer constructor.
  IUnknown::RefPtr unknown ( pm.findFirstIf ( Helper::CanCreate ( name ) ) );
  IDocumentCreate::QueryPtr creator ( unknown );
  if ( false == creator.valid() )
    return IUnknown::RefPtr ( 0x0 );

  // Create and return the document.
  IUnknown::RefPtr doc ( creator->createDocument ( caller ) );
  return doc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return all filters from plugins that create documents.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::filters ( const std::string &type, Filters &filters ) const
{
  typedef Usul::Plugins::Manager::UnknownSet UnknownSet;
  typedef Usul::Interfaces::IDocumentFilters IDocumentFilters;
  typedef Usul::Interfaces::IDocumentCreate IDocumentCreate;

  // First, get all plugins that can create documents. If we just ask for 
  // filters directly then we get also plugins that do not create documents.
  UnknownSet plugins;
  Usul::Plugins::Manager::instance().findAll ( IDocumentCreate::IID, plugins );

  // Now that we have all the document creators, ask them for their filters.
  for ( UnknownSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    IDocumentFilters::QueryPtr f ( *i );
    if ( true == f.valid() )
    {
      f->appendDocumentFilters ( type, filters );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Look for a document with the given title.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Manager::find ( const std::string &title ) const
{
  typedef Usul::Interfaces::IDocumentGet IDocumentGet;
  typedef Usul::Interfaces::IUnknown IUnknown;

  DocumentSequence docs ( _documents );
  for ( DocumentSequence::iterator i = docs.begin(); i != docs.end(); ++i )
  {
    IDocumentGet::QueryPtr idoc ( *i );
    if ( true == idoc.valid() )
    {
      const Document::RefPtr doc ( idoc->documentGet() );
      if ( true == doc.valid() )
      {
        if ( title == doc->title() )
        {
          return IUnknown::RefPtr ( idoc );
        }
      }
    }
  }
  return IUnknown::RefPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the document.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addDocument ( IUnknown::RefPtr doc )
{
  // If it's valid...
  if ( true == doc.valid() )
  {
    // Lock and get shortcut.
    DocumentCollection::Guard guard ( _documents.mutex() );
    DocumentSequence &d ( _documents.getReference() );

    // Do not add more than once.
    if ( d.end() == std::find ( d.begin(), d.end(), doc ) )
    {
      d.push_back ( doc );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper predicate for removing documents.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct HasRefCount
  {
    typedef Usul::Base::Referenced::RefCount RefCount;
    typedef Usul::Interfaces::IReferenceCount IReferenceCount;
    HasRefCount ( RefCount count ) : _count ( count )
    {
      USUL_ASSERT_SAME_TYPE ( RefCount, IReferenceCount::RefCount );
    }
    bool operator () ( Manager::DocumentSequence::reference doc ) const
    {
      // This will increase the count by one.
      Usul::Interfaces::IReferenceCount::QueryPtr rc ( doc );
      const bool state ( ( true == rc.valid() ) ? ( _count + 1 ) == rc->getReferenceCount() : false );
      return state;
    }
  private:
    const RefCount _count;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge all documents with a reference count of one.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::purge()
{
  // This will hold the documents to remove.
  DocumentSequence removeMe;

  // Local scope to lock and copy all documents ready for removal.
  {
    // Lock and get shortcut.
    DocumentCollection::Guard guard ( _documents.mutex() );
    DocumentSequence &d ( _documents.getReference() );

    // Put all documents ready to be removed at the end of the sequence.
    DocumentSequence::iterator middle ( std::stable_partition 
      ( d.begin(), d.end(), Helper::HasRefCount ( 1 ) ) );

    // Copy to local vector and erase from member.
    removeMe.assign ( d.begin(), middle );
    d.erase ( d.begin(), middle );
  }

  // Loop through the list to be removed.
  for ( DocumentSequence::iterator i = removeMe.begin(); i != removeMe.end(); ++i )
  {
    // Feedback if we can.
    Usul::Interfaces::IDocumentGet::QueryPtr getter ( *i );
    Usul::Documents::Document::RefPtr doc ( 
      ( true == getter.valid() ) ? getter->documentGet() : 0x0 );
    if ( true == doc.valid() )
    {
      std::cout << Usul::Strings::format ( "Closed document: ", doc->title(), '\n' ) << std::flush;
    }

    // Explicitely destroy the object if we can.
    Usul::Interfaces::IDestroyObject::QueryPtr destroy ( *i );
    if ( true == destroy.valid() )
    {
      destroy->destroyObject();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make given document active.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_setActiveDocument ( IUnknown::RefPtr newDoc )
{
  // Set the new document and get the old one.
  IUnknown::RefPtr oldDoc ( _activeDoc.fetchAndStore ( newDoc ) );

  // Return now if same.
  if ( newDoc == oldDoc )
    return;

  // Notify the listeners.
  this->_notifyListenersActiveDoc ( oldDoc, newDoc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make given view's document active.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_setActiveDocumentFromView ( IUnknown::RefPtr newView )
{
  Usul::Interfaces::IDocumentGet::QueryPtr idoc ( newView );
  Usul::Documents::Document::RefPtr newDoc ( ( true == idoc.valid() ) ? idoc->documentGet() : 0x0 );
  this->_setActiveDocument ( Usul::Interfaces::IUnknown::QueryPtr ( newDoc ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Manager::activeDocumentGet()
{
  return _activeDoc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make given view active.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeViewSet ( IUnknown::RefPtr newView )
{
  // Pass predicate that evaluates to true if the existing view is not newView.
  // This means we only set newView if it isn't already the active view.
  this->activeViewSetIf ( newView, std::bind2nd ( std::not_equal_to < IUnknown::RefPtr >(), newView ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the listeners that the active view changed.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_notifyListenersActiveView ( IUnknown::RefPtr oldView, IUnknown::RefPtr newView )
{
  typedef ActiveViewListeners::ValueType Listeners;
  typedef Listeners::iterator Itr;
  Listeners listeners ( _activeViewListeners );
  for ( Itr i = listeners.begin(); i != listeners.end(); ++i )
  {
    IActiveViewListener::RefPtr listener ( *i );
    if ( true == listener.valid() )
    {
      Usul::Functions::noThrow ( boost::bind ( &IActiveViewListener::activeViewChanged, listener.get(), oldView, newView ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the listeners that the active document changed.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_notifyListenersActiveDoc ( IUnknown::RefPtr oldDoc, IUnknown::RefPtr newDoc )
{
  typedef ActiveDocumentListeners::ValueType Listeners;
  typedef Listeners::iterator Itr;
  Listeners listeners ( _activeDocListeners );
  for ( Itr i = listeners.begin(); i != listeners.end(); ++i )
  {
    IActiveDocumentListener::RefPtr listener ( *i );
    if ( true == listener.valid() )
    {
      Usul::Functions::noThrow ( boost::bind ( &IActiveDocumentListener::activeDocumentChanged, listener.get(), oldDoc, newDoc ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active view.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Manager::activeViewGet()
{
  return _activeView;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeDocumentListenerAdd ( IUnknown::RefPtr unknown )
{
  IActiveDocumentListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    ActiveDocumentListeners::Guard guard ( _activeDocListeners.mutex() );
    _activeDocListeners.getReference().push_back ( listener );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeDocumentListenerRemove ( IUnknown::RefPtr unknown )
{
  IActiveDocumentListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    ActiveDocumentListeners::Guard guard ( _activeDocListeners.mutex() );
    _activeDocListeners.getReference().remove ( listener );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeViewListenerAdd ( IUnknown::RefPtr unknown )
{
  IActiveViewListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    ActiveViewListeners::Guard guard ( _activeViewListeners.mutex() );
    _activeViewListeners.getReference().push_back ( listener );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeViewListenerRemove ( IUnknown::RefPtr unknown )
{
  IActiveViewListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    ActiveViewListeners::Guard guard ( _activeViewListeners.mutex() );
    _activeViewListeners.getReference().remove ( listener );
  }
}
