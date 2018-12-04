
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Event and supporting classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_EVENT_CLASSES_H_
#define _SCENE_GRAPH_EVENT_CLASSES_H_

#include "SceneGraph/Base/Object.h"

#include <set>


namespace SceneGraph {
namespace Events {


///////////////////////////////////////////////////////////////////////////////
//
//  Base event class.
//
///////////////////////////////////////////////////////////////////////////////

class Event : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_EVENT ( Event, SceneGraph::Base::Object );

  typedef std::set<int> KeysDown;
  typedef std::set<unsigned int> ButtonsDown;

  KeysDown keysDown() const
  {
    return _keys;
  }

  ButtonsDown buttonsDown() const
  {
    return _buttons;
  }

protected:

  Event ( const KeysDown &k, const ButtonsDown &b ) : 
    _keys ( k ), _buttons ( b )
  {
  }

  virtual ~Event()
  {
  }

private:

  const KeysDown _keys;
  const ButtonsDown _buttons;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Base mouse event class.
//
///////////////////////////////////////////////////////////////////////////////

class MouseEvent : public Event
{
public:

  SCENE_GRAPH_EVENT ( MouseEvent, Event );

  MouseEvent ( double x, double y, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( k, b ), _x ( x ), _y ( y )
  {
  }

  double x() const
  {
    return _x;
  }

  double y() const
  {
    return _y;
  }

protected:

  virtual ~MouseEvent()
  {
  }

private:

  const double _x;
  const double _y;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse move event class.
//
///////////////////////////////////////////////////////////////////////////////

class MouseMove : public MouseEvent
{
public:

  SCENE_GRAPH_EVENT ( MouseMove, MouseEvent );

  MouseMove ( double x, double y, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( x, y, k, b )
  {
  }

protected:

  virtual ~MouseMove()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Button press event class.
//
///////////////////////////////////////////////////////////////////////////////

class MousePress : public MouseEvent
{
public:

  SCENE_GRAPH_EVENT ( MousePress, MouseEvent );

  MousePress ( double x, double y, unsigned int p, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( x, y, k, b ), _button ( p )
  {
  }

  unsigned int button() const
  {
    return _button;
  }

protected:

  virtual ~MousePress()
  {
  }

private:

  const unsigned int _button;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse release event class.
//
///////////////////////////////////////////////////////////////////////////////

class MouseRelease : public MouseEvent
{
public:

  SCENE_GRAPH_EVENT ( MouseRelease, MouseEvent );

  MouseRelease ( double x, double y, unsigned int r, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( x, y, k, b ), _button ( r )
  {
  }

  unsigned int button() const
  {
    return _button;
  }

protected:

  virtual ~MouseRelease()
  {
  }

private:

  const unsigned int _button;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse double-click event class.
//
///////////////////////////////////////////////////////////////////////////////

class MouseDoubleClick : public MouseEvent
{
public:

  SCENE_GRAPH_EVENT ( MouseDoubleClick, MouseEvent );

  MouseDoubleClick ( double x, double y, unsigned int c, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( x, y, k, b ), _button ( c )
  {
  }

  unsigned int button() const
  {
    return _button;
  }

protected:

  virtual ~MouseDoubleClick()
  {
  }

private:

  const unsigned int _button;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse wheel event class.
//
///////////////////////////////////////////////////////////////////////////////

class MouseWheel : public MouseEvent
{
public:

  SCENE_GRAPH_EVENT ( MouseWheel, MouseEvent );

  MouseWheel ( double x, double y, double d, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( x, y, k, b ), _delta ( d )
  {
  }

  double delta() const
  {
    return _delta;
  }

protected:

  virtual ~MouseWheel()
  {
  }

private:

  const double _delta;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Key press event class.
//
///////////////////////////////////////////////////////////////////////////////

class KeyPress : public Event
{
public:

  SCENE_GRAPH_EVENT ( KeyPress, Event );

  KeyPress ( int c, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( k, b ), _key ( c )
  {
  }

  int key() const
  {
    return _key;
  }

protected:

  virtual ~KeyPress()
  {
  }

private:

  const int _key;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Key release event class.
//
///////////////////////////////////////////////////////////////////////////////

class KeyRelease : public Event
{
public:

  SCENE_GRAPH_EVENT ( KeyRelease, Event );

  KeyRelease ( int c, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( k, b ), _key ( c )
  {
  }

  int key() const
  {
    return _key;
  }

protected:

  virtual ~KeyRelease()
  {
  }

private:

  const int _key;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Paint event class.
//
///////////////////////////////////////////////////////////////////////////////

class Paint : public Event
{
public:

  SCENE_GRAPH_EVENT ( Paint, Event );

  Paint ( const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( k, b )
  {
  }

protected:

  virtual ~Paint()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Resize event class.
//
///////////////////////////////////////////////////////////////////////////////

class Resize : public Event
{
public:

  SCENE_GRAPH_EVENT ( Resize, Event );

  Resize ( double w, double h, const KeysDown &k = KeysDown(), const ButtonsDown &b = ButtonsDown() ) : 
    BaseClass ( k, b ), _width ( w ), _height ( h )
  {
  }

  double height() const
  {
    return _height;
  }

  double width() const
  {
    return _width;
  }

protected:

  virtual ~Resize()
  {
  }

  const double _width;
  const double _height;
};


}; // namespace Events
}; // namespace SceneGraph


#endif // _SCENE_GRAPH_EVENT_CLASSES_H_
