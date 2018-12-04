
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Manager for shaders.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Shaders/Manager.h"

#include "Usul/Functions/NoThrow.h"

using namespace SceneGraph::Shaders;


///////////////////////////////////////////////////////////////////////////////
//
//  Variables with file scope.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  SceneGraph::Shaders::Manager *instance ( 0x0 );
  Usul::Threads::Mutex mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() : 
  _mutex(),
  _programs()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  USUL_TRY_BLOCK
  {
    _programs.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "2065423038" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the singleton instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager &Manager::instance()
{
  Guard guard ( Helper::mutex );
  if ( 0x0 == Helper::instance )
  {
    Helper::instance = new Manager();
  }
  return *Helper::instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a program and default parameters.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addProgram ( const std::string &name, 
                           const std::string &vertexCode,
                           const std::string &fragmentCode,
                           const Parameters &params )
{
  if ( ( false == name.empty() ) && 
       ( false == vertexCode.empty() ) && 
       ( false == fragmentCode.empty() ) )
  {
    Guard guard ( _mutex );
    Program::RefPtr program ( new Program ( name, vertexCode, fragmentCode ) );
    _programs[name] = ProgramAndParams ( program, params );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the program.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeProgram ( const std::string &name )
{
  Guard guard ( _mutex );
  _programs.erase ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all programs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeAllPrograms()
{
  Guard guard ( _mutex );
  _programs.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the program.
//
///////////////////////////////////////////////////////////////////////////////

Manager::ProgramAndParams Manager::_getPair ( const std::string &name ) const
{
  Guard guard ( _mutex );
  ProgramMap::const_iterator i ( _programs.find ( name ) );
  return ( ( _programs.end() != i ) ? i->second : ProgramAndParams() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new shader with the program name.
//
///////////////////////////////////////////////////////////////////////////////

Shader::RefPtr Manager::createShader ( const std::string &name )
{
  Guard guard ( _mutex );

  // Get the program and parameters.
  ProgramAndParams pp ( this->_getPair ( name ) );
  Program::RefPtr program ( pp.first );
  if ( false == program.valid() )
  {
    return Shader::RefPtr();
  }

  // Make a copy of the default parameters.
  Shader::Parameters parameters;
  typedef Shader::Parameters::const_iterator Itr;
  for ( Itr i = pp.second.begin(); i != pp.second.end(); ++i )
  {
    const std::string name ( i->first );
    if ( false == name.empty() )
    {
      Shaders::BaseParameter::RefPtr p ( i->second );
      if ( true == p.valid() )
      {
        // Clone this parameter because each shader should have its own copy.
        parameters[name] = p->clone();
      }
    }
  }

  // Return a new shader.
  return Shader::RefPtr ( new Shader ( program, parameters ) );
}
