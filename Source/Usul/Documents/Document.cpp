
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

#include "Usul/Documents/Document.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Interfaces/IDocumentView.h"
#include "Usul/MPL/SameType.h"

#include "boost/bind.hpp"

#include <algorithm>

using namespace Usul::Documents;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Document, BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs with file scope.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Mutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Document::Document() : BaseClass(),
  _modified ( false ),
  _views(),
  _modifiedListeners()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Document::~Document()
{
  Usul::Functions::noThrow ( boost::bind ( &Document::_destroy, this ), "2498408519" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_destroy()
{
  // Should not have any views.
  if ( false == _views.empty() )
  {
    USUL_ASSERT ( false );
    _views.clear();
  }

  // Clear the listeners.
  {
    Guard guard ( _modifiedListeners.mutex() );
    _modifiedListeners.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Document::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IDocumentGet::IID:
    return static_cast< Usul::Interfaces::IDocumentGet*> ( this );
  case Usul::Interfaces::IClearObject::IID:
    return static_cast< Usul::Interfaces::IClearObject*> ( this );
  case Usul::Interfaces::IDestroyObject::IID:
    return static_cast< Usul::Interfaces::IDestroyObject*> ( this );
  case Usul::Interfaces::IModifiedSubject::IID:
    return static_cast< Usul::Interfaces::IModifiedSubject*> ( this );
  case Usul::Interfaces::IReferenceCount::IID:
    return static_cast< Usul::Interfaces::IReferenceCount*> ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Document::destroyObject()
{
  this->_destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Document::clearObject()
{
  // Intentionally not clearing members.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this document.
//
///////////////////////////////////////////////////////////////////////////////

const Document *Document::documentGet() const
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this document.
//
///////////////////////////////////////////////////////////////////////////////

Document *Document::documentGet()
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a string suitable for a GUI window's title bar.
//
///////////////////////////////////////////////////////////////////////////////

std::string Document::title() const
{
  return std::string ( "Default Title" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Document::modifiedListenerAdd ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  IModifiedListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    // Do not check for duplicates.
    Guard guard ( _modifiedListeners.mutex() );
    _modifiedListeners.getReference().push_back ( listener );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Document::modifiedListenerRemove ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  IModifiedListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    // Just erase first one because there could be duplicates.
    Guard guard ( _modifiedListeners.mutex() );
    ModifiedListeners &v ( _modifiedListeners.getReference() );
    v.erase ( std::find ( v.begin(), v.end(), listener ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the document modified?
//
///////////////////////////////////////////////////////////////////////////////

bool Document::isModified() const
{
  return _modified;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the modified state.
//
///////////////////////////////////////////////////////////////////////////////

void Document::modified ( bool state )
{
  // Set the new value and return the previous one.
  const bool previous ( _modified.fetch_and_store ( state ) );

  // Notify the listeners if the state changed.
  if ( previous != state )
  {
    this->_notifyModifiedListeners();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the modified-listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_notifyModifiedListeners()
{
  // Copy the list.
  ModifiedListeners listeners ( _modifiedListeners );

  // If there are no listeners then punt.
  if ( true == listeners.empty() )
    return;

  // Needed below.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Loop through the list.
  typedef ModifiedListeners::iterator Itr;
  for ( Itr i = listeners.begin(); i != listeners.end(); ++i )
  {
    // Notify the listener.
    IModifiedListener::RefPtr listener ( *i );
    if ( true == listener.valid() )
    {
      listener->subjectModified ( me );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the reference count.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Document::getReferenceCount() const
{
  USUL_ASSERT_SAME_TYPE ( unsigned long, Usul::Interfaces::IReferenceCount::RefCount );
  USUL_ASSERT_SAME_TYPE ( unsigned long, BaseClass::RefCount );
  USUL_ASSERT_SAME_TYPE ( unsigned long, Usul::Base::Referenced::RefCount );
  return this->refCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the view.
//
///////////////////////////////////////////////////////////////////////////////

void Document::viewAdd ( IUnknown::RefPtr view )
{
  if ( true == view.valid() )
  {
    // Do not check for duplicates.
    Guard guard ( _views.mutex() );
    _views.getReference().push_back ( view );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the view.
//
///////////////////////////////////////////////////////////////////////////////

void Document::viewRemove ( IUnknown::RefPtr view )
{
  if ( true == view.valid() )
  {
    // Just erase first one because there could be duplicates.
    Guard guard ( _views.mutex() );
    Views &v ( _views.getReference() );
    v.erase ( std::find ( v.begin(), v.end(), view ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of views.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Document::viewCount() const
{
  return _views.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save this document.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::save()
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bring this document's views to the front.
//
///////////////////////////////////////////////////////////////////////////////

void Document::viewsForward()
{
  typedef Usul::Interfaces::IDocumentView IDocumentView;

  // Copy the views.
  Views views ( _views );

  // The first view will be on top if the order is reversed.
  typedef Views::reverse_iterator Itr;
  for ( Itr i = views.rbegin(); i != views.rend(); ++i )
  {
    IDocumentView::QueryPtr view ( *i );
    if ( true == view.valid() )
    {
      view->bringToFront();
    }
  }
}
