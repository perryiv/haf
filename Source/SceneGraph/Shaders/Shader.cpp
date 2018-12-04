
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

#include "SceneGraph/Shaders/Shader.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Strings/Format.h"

using namespace SceneGraph::Shaders;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Shader::Shader ( const Program::RefPtr program, 
                 const Parameters &params ) : BaseClass(),
  _program ( program ),
  _params ( params )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Shader::~Shader()
{
  USUL_TRY_BLOCK
  {
    _program = Program::RefPtr();
    _params.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "4053136130" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name of the program.
//
///////////////////////////////////////////////////////////////////////////////

std::string Shader::name() const
{
  Program::RefPtr program ( _program );
  return ( ( true == program.valid() ) ? program->name() : std::string() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the named parameter.
//
///////////////////////////////////////////////////////////////////////////////

boost::any Shader::paramGet ( const std::string &name ) const
{
  Guard guard ( _params.mutex() );
  const Parameters &params ( _params.getReference() );
  Parameters::const_iterator i ( params.find ( name ) );
  if ( params.end() == i )
  {
    throw Usul::Exceptions::Error ( "3350069760", Usul::Strings::format
      ( "No parameter '", name, "' in shader '", this->name(), "'" ) );
  }

  BaseParameter::RefPtr param ( i->second );
  if ( false == param.valid() )
  {
    // This might indicate an internal error in logic.
    throw Usul::Exceptions::Error ( "2865169660", Usul::Strings::format
      ( "Parameter '", name, "' is null in shader '", this->name(), "'" ) );
  }

  return param->get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the named parameter.
//
///////////////////////////////////////////////////////////////////////////////

void Shader::paramSet ( const std::string &name, const boost::any &value )
{
  Guard guard ( _params.mutex() );
  const Parameters &params ( _params.getReference() );
  Parameters::const_iterator i ( params.find ( name ) );

  // Ignore names not in the map. This is not necessarily an error.
  if ( params.end() == i )
    return;

  // Should not happen but it's not the caller's fault.
  BaseParameter::RefPtr param ( i->second );
  if ( false == param.valid() )
    return;

  // Set the value.
  param->set ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the names of the Set parameters.
//
///////////////////////////////////////////////////////////////////////////////

Shader::ParamNames Shader::paramNames() const
{
  ParamNames names;
  this->paramNames ( names );
  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the program.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraph::Shaders::Program::RefPtr Shader::programGet()
{
  return _program;
}
