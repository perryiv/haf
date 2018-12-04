
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Shader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_SHADER_CLASS_H_
#define _SCENE_GRAPH_SHADER_CLASS_H_

#include "SceneGraph/Shaders/Parameters.h"
#include "SceneGraph/Shaders/Program.h"
#include "SceneGraph/State/Attributes/BaseAttribute.h"

#include "Usul/Atomic/Container.h"

#include "boost/any.hpp"

#include <map>
#include <string>
#include <vector>


namespace SceneGraph {
namespace Shaders {


///////////////////////////////////////////////////////////////////////////////
//
//  Shader class.
//
///////////////////////////////////////////////////////////////////////////////

class SCENE_GRAPH_EXPORT Shader : public SceneGraph::State::Attributes::ExclusiveAttribute
{
public:

  SCENE_GRAPH_ATTRIBUTE ( Shader, SceneGraph::State::Attributes::ExclusiveAttribute );
  typedef std::map < std::string, BaseParameter::RefPtr > Parameters;
  typedef std::vector < std::string > ParamNames;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;

  // Construction.
  Shader ( Program::RefPtr program, const Parameters &params );

  std::string               name() const;

  ParamNames                paramNames() const;
  template < class C > void paramNames ( C & ) const;

  boost::any                paramGet ( const std::string &name ) const;
  template < class T > void paramSet ( const std::string &name, const T &value );
  void                      paramSet ( const std::string &name, const boost::any &value );

  Program::RefPtr           programGet();

protected:

  // Use reference counting.
  virtual ~Shader();

private:

  Usul::Atomic::Object < Program::RefPtr > _program;
  Usul::Atomic::Container < Parameters > _params;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the parameter.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > void Shader::paramSet ( const std::string &n, const T &v )
{
  this->paramSet ( n, boost::any ( v ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the names of the parameters.
//
///////////////////////////////////////////////////////////////////////////////

template < class C > void Shader::paramNames ( C &c ) const
{
  Guard guard ( _params.mutex() );
  const Parameters &params ( _params.getReference() );
  typedef Parameters::const_iterator Itr;
  for ( Itr i = params.begin(); i != params.end(); ++i )
  {
    c.insert ( c.end(), i->first );
  }
}


} // namespace Shaders
} // namespace SceneGraph


#endif // _SCENE_GRAPH_SHADER_CLASS_H_
