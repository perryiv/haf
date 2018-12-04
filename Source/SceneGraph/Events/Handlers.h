
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Event handler classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_EVENT_HANDLERS_CLASSES_H_
#define _SCENE_GRAPH_EVENT_HANDLERS_CLASSES_H_

#include "SceneGraph/Events/Events.h"


namespace SceneGraph {
namespace Events {


///////////////////////////////////////////////////////////////////////////////
//
//  Base handler class.
//
///////////////////////////////////////////////////////////////////////////////

class BaseHandler : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( BaseHandler, SceneGraph::Base::Object );

  virtual void handle ( Event::RefPtr )
  {
  }

protected:

  BaseHandler()
  {
  }

  virtual ~BaseHandler()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete handler class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class Handler : public BaseHandler
{
public:

  SCENE_GRAPH_OBJECT ( Handler, BaseHandler );

  Handler ( Function f ) : _function ( f )
  {
  }

  virtual void handle ( Event::RefPtr e )
  {
    _function ( e );
  }

protected:

  virtual ~Handler()
  {
  }

  Function _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for making handlers.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > inline BaseHandler::RefPtr makeHandler ( Function fun )
{
  typedef Handler < Function > HandlerType;
  return BaseHandler::RefPtr ( new HandlerType ( fun ) );
}


}; // namespace Events
}; // namespace SceneGraph


#endif // _SCENE_GRAPH_EVENT_HANDLERS_CLASSES_H_
