
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes that contain shader parameters.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHADER_PARAMETER_CLASSES_H_
#define _SCENE_GRAPH_SHADER_PARAMETER_CLASSES_H_

#include "SceneGraph/Base/Object.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Strings/Format.h"

#include "boost/any.hpp"

#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Overloads of string format function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Strings
  {
    inline std::string format ( const Usul::Math::Vec2f &v )
    {
      return Usul::Strings::format ( v[0], ' ', v[1] );
    }
    inline std::string format ( const Usul::Math::Vec3f &v )
    {
      return Usul::Strings::format ( v[0], ' ', v[1], ' ', v[2] );
    }
    inline std::string format ( const Usul::Math::Vec4f &v )
    {
      return Usul::Strings::format ( v[0], ' ', v[1], ' ', v[2], ' ', v[3] );
    }
  }
}


namespace SceneGraph {
namespace Shaders {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class.
//
///////////////////////////////////////////////////////////////////////////////

class BaseParameter : public SceneGraph::Base::Object
{
public:

  SCENE_GRAPH_OBJECT ( BaseParameter, SceneGraph::Base::Object );

  virtual boost::any get() const
  {
    return boost::any();
  }

  const std::string &name() const
  {
    return _name;
  }

  virtual BaseParameter::RefPtr clone() const
  {
    return RefPtr();
  }

  virtual void set ( const boost::any &value )
  {
  }

  template < class T > void set ( const T &value )
  {
    this->set ( boost::any ( value ) );
  }

  virtual std::string toString() const
  {
    return std::string();
  }

protected:

  BaseParameter ( const std::string &name ) : BaseClass(),
    _name ( name )
  {
  }

  virtual ~BaseParameter()
  {
  }

private:

  const std::string _name;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete parameter class.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class Parameter : public BaseParameter
{
public:

  SCENE_GRAPH_OBJECT ( Parameter, BaseParameter );

  Parameter ( const std::string &name ) : BaseClass ( name ), 
    _value()
  {
  }

  Parameter ( const std::string &name, const T &value ) : BaseClass ( name ), 
    _value ( value )
  {
  }

  virtual BaseClass::RefPtr clone() const
  {
    return BaseClass::RefPtr ( new Parameter ( this->name(), _value ) );
  }

  virtual boost::any get() const
  {
    return boost::any ( _value );
  }

  virtual void set ( const boost::any &value )
  {
    // For an explanation of why it's done this way:
    // http://groups.google.com/group/boost-list/browse_thread/thread/c3a20712315fd82a/0add80ebca0fe84a?lnk=raot
    try
    {
      if ( 0 == ::strcmp ( typeid ( T ).name(), value.type().name() ) )
      {
        const T *value ( boost::unsafe_any_cast < T > ( &value ) );
        if ( 0x0 != value )
        {
          _value = *value;
        }
      }
    }
    catch ( ... )
    {
    }
  }

  virtual std::string toString() const
  {
    return Usul::Strings::format ( _value );
  }

protected:

  virtual ~Parameter()
  {
  }

private:

  T _value;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
inline BaseParameter::RefPtr makeParameter ( const std::string &name )
{
  return BaseParameter::RefPtr ( new Parameter < T > ( name ) );
}
template < class T >
inline BaseParameter::RefPtr makeParameter ( const std::string &n, const T &v )
{
  return BaseParameter::RefPtr ( new Parameter < T > ( n, v ) );
}


} // namespace Shaders
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHADER_PARAMETER_CLASSES_H_
