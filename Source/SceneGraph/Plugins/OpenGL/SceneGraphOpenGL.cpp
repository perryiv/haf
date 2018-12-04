
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Plugins/OpenGL/SceneGraphOpenGL.h"
#include "SceneGraph/Interfaces/IContext.h"
#include "SceneGraph/Plugins/OpenGL/DrawOpenGL.h"
#include "SceneGraph/Shaders/Manager.h"
#include "SceneGraph/Shaders/Parameters.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Convert/Vector3.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/File/AsciiInputFile.h"
#include "Usul/File/Find.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Plugins/Helper.h"
#include "Usul/Registry/Database.h"
#include "Usul/System/Environment.h"

#include "boost/algorithm/string/regex.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem.hpp"
#include "boost/function.hpp"
#include "boost/regex.hpp"

#include <list>
#include <string>

using namespace SceneGraph::OpenGL;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneGraphOpenGL, BaseClass );
USUL_PLUGIN_INITIALIZE ( SceneGraphOpenGL );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphOpenGL::SceneGraphOpenGL() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphOpenGL::~SceneGraphOpenGL()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneGraphOpenGL::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case SceneGraph::Interfaces::IDrawMethodCreate::IID:
    return static_cast < SceneGraph::Interfaces::IDrawMethodCreate * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to execute the given function in a worker thread if that 
//  queue is available. Otherwise it executes in the calling thread.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class F > inline void execute ( F f, const char *errorId )
  {
    // If we have a worker queue then use it.
    Usul::Jobs::Manager &manager ( Usul::Jobs::Manager::instance() );
    Usul::Jobs::Queue::RefPtr &queue ( manager["worker_queue"] );
    if ( true == queue.valid() )
    {
      queue->add ( f );
    }

    // Otherwise, execute in this thread.
    else
    {
      Usul::Functions::noThrow ( f, errorId );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to flush the stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void flushStream ( std::ostream &out )
  {
    out << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::pluginInitialize()
{
  // This will queue the job for reading shader files.
  Helper::execute ( boost::bind 
    ( &SceneGraphOpenGL::_addShaders, this ), "5141455680" );

  // This will queue a job to flush the buffer. Because of the way the queue 
  // works this will be called after the above job.
  Helper::execute ( boost::bind 
    ( Helper::flushStream, boost::ref ( std::cout ) ), "1497621840" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the plugin is released.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::pluginFinalize()
{
  SceneGraph::Shaders::Manager::instance().removeAllPrograms();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add all the shaders.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::_addShaders()
{
  // Determine the directories to look for shader files. Order of preference 
  // is 1) command-line, 2) environment variable, and 3) the directory where 
  // the executable resides.
  typedef Usul::Registry::Database Reg;
  Reg &reg ( Reg::instance(false) );
  std::string path ( reg["shader_path"].get ( std::string(), false ) );
  if ( true == path.empty() )
  {
    typedef Usul::System::Environment Env;
    path = Env::get ( "HAF_SCENE_GRAPH_OPENGL_SHADER_PATH" );
    if ( true == path.empty() )
    {
      typedef Usul::CommandLine::Arguments Args;
      boost::filesystem::path p ( Args::instance().get ( 0 ) );
      p = boost::filesystem::complete ( p );
      path = p.parent_path().string();
    }
  }

  // Punt if we didn't find the shader path.
  if ( true == path.empty() )
    return;

  // Split the path into directories.
  typedef std::list < std::string > Dirs;
  Dirs dirs;
  boost::algorithm::split ( dirs, path, 
    std::bind1st ( std::equal_to<char>(), ';' ) );

  // Loop through all the directories.
  for ( Dirs::const_iterator i = dirs.begin(); i != dirs.end(); ++i )
  {
    // Add the shaders from this directory.
    const std::string dir ( *i );
    Usul::Functions::noThrow ( boost::bind 
      ( &SceneGraphOpenGL::_addShaders, this, dir ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add all the shaders.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::_addShaders ( const std::string &dir )
{
  // Make sure it exists.
  if ( false == boost::filesystem::exists ( dir ) )
    return;

  // Find all the shader files. For a nice collection of shaders see:
  // http://qshaderedit.googlecode.com/svn/trunk/shaders/
  typedef std::list < std::string > Files;
  Files files;
  Usul::File::Find::HasExtension pred ( ".glsl" );
  Usul::File::Find::find ( dir, pred, true, files );

  // Loop through the files and add the shader.
  for ( Files::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    const std::string &file ( *i );
    Usul::Functions::noThrow ( boost::bind 
      ( &SceneGraphOpenGL::_addShader, this, file ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shader to the manager.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::_addShader ( const std::string &file )
{
  // Check file.
  if ( false == boost::filesystem::exists ( file ) )
    return;

  // Read the contents of the file.
  typedef Usul::File::AsciiInputFile File;
  File::RefPtr in ( new File ( file ) );
  std::string contents;
  in->read ( contents );

  // Remove carriage-return characters.
  boost::algorithm::replace_all ( contents, "\r\n", "\n" );
  std::replace ( contents.begin(), contents.end(), '\r', '\n' );

  // Split the contents into lines.
  typedef std::list < std::string > Lines;
  Lines lines;
  boost::algorithm::split ( lines, contents, 
    std::bind1st ( std::equal_to<char>(), '\n' ) );

  // Punt if no lines.
  while ( true == lines.empty() )
    return;

  // Discard lines until we find the beginning of the vertex program.
  while ( ( false == lines.empty() ) && ( "[VertexShader]" != lines.front() ) )
  {
    lines.pop_front();
  }

  // Punt if no lines.
  while ( true == lines.empty() )
    return;

  // Discard the current line.
  lines.pop_front();

  // Append lines to vertex program until we see the fragment shader.
  std::string vertexProgram;
  while ( ( false == lines.empty() ) && ( "[FragmentShader]" != lines.front() ) )
  {
    vertexProgram.append ( lines.front() + "\n" );
    lines.pop_front();
  }

  // Punt if no lines.
  while ( true == lines.empty() )
    return;

  // Discard the current line.
  lines.pop_front();

  // Append lines to fragment program until we see the parameters.
  std::string fragmentProgram;
  while ( ( false == lines.empty() ) && ( "[Parameters]" != lines.front() ) )
  {
    fragmentProgram.append ( lines.front() + "\n" );
    lines.pop_front();
  }

  // Punt if no lines.
  while ( true == lines.empty() )
    return;

  // Discard the current line.
  lines.pop_front();

  // Process remaining lines as parameters.
  Parameters parameters;
  while ( false == lines.empty() )
  {
    this->_addParameter ( lines.front(), parameters );
    lines.pop_front();
  }

  // Get shader manager.
  typedef SceneGraph::Shaders::Manager ShaderManager;
  ShaderManager &sm ( ShaderManager::instance() );

  // Add the shader.
  const std::string name ( boost::filesystem::basename ( file ) );
  sm.addProgram ( name, vertexProgram, fragmentProgram, parameters );

  // Feedback.
  std::cout << Usul::Strings::format ( "Added shader: ", file, '\n' );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the parameter.
//
///////////////////////////////////////////////////////////////////////////////

void SceneGraphOpenGL::_addParameter ( const std::string &input, Parameters &p )
{
  // Lose leading and trailing white space.
  std::string line ( boost::algorithm::trim_copy ( input ) );

  // Ignore empty lines.
  if ( true == line.empty() )
    return;

  // Should end in a semicolin.
  if ( ';' != line.at ( line.size() - 1 ) )
    return;

  // Drop the semicolin.
  line.erase ( line.size() - 1 );

  // Split the line at the equal sign.
  typedef std::list < std::string > Parts;
  Parts parts;
  boost::algorithm::split ( parts, line, 
    std::bind1st ( std::equal_to<char>(), '=' ) );

  // Should be two parts.
  if ( 2 != parts.size() )
    return;

  // Save the two parts.
  const std::string left ( boost::algorithm::trim_copy ( parts.front() ) );
  std::string right ( boost::algorithm::trim_copy ( parts.back()  ) );

  // Split the left.
  boost::algorithm::split ( parts, left, 
    std::bind1st ( std::equal_to<char>(), ' ' ) );

  // Should be two parts.
  if ( 2 != parts.size() )
    return;

  // Save the two parts.
  const std::string type ( parts.front() );
  const std::string variable ( parts.back() );

  // Make the string more easily parsed.
  boost::algorithm::replace_all ( right, "vec2", " " );
  boost::algorithm::replace_all ( right, "vec3", " " );
  boost::algorithm::replace_all ( right, "vec4", " " );
  boost::algorithm::replace_all ( right, "(", " " );
  boost::algorithm::replace_all ( right, ")", " " );
  boost::algorithm::replace_all ( right, ",", " " );
  boost::algorithm::trim ( right );

  // Parse the rest of the line depending on the type.
  if ( "float" == type )
  {
    typedef Usul::Convert::Type < std::string, float > Converter;
    p[variable] = SceneGraph::Shaders::makeParameter < float > 
      ( variable, Converter::convert ( right ) );
  }
  else if ( "vec2" == type )
  {
    typedef Usul::Math::Vec2f Vec;
    typedef Usul::Convert::Type < std::string, Vec > Converter;
    p[variable] = SceneGraph::Shaders::makeParameter < Vec > 
      ( variable, Converter::convert ( right ) );
  }
  else if ( "vec3" == type )
  {
    typedef Usul::Math::Vec3f Vec;
    typedef Usul::Convert::Type < std::string, Vec > Converter;
    p[variable] = SceneGraph::Shaders::makeParameter < Vec > 
      ( variable, Converter::convert ( right ) );
  }
  else if ( "vec4" == type )
  {
    typedef Usul::Math::Vec4f Vec;
    typedef Usul::Convert::Type < std::string, Vec > Converter;
    p[variable] = SceneGraph::Shaders::makeParameter < Vec > 
      ( variable, Converter::convert ( right ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
///////////////////////////////////////////////////////////////////////////////

std::string SceneGraphOpenGL::pluginDescription() const
{
  return std::string ( "Uses OpenGL to render the scene graph" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new draw-method.
//
///////////////////////////////////////////////////////////////////////////////

SceneGraphOpenGL::DrawMethodPtr SceneGraphOpenGL::createDrawMethod 
  ( IUnknown::RefPtr unknown )
{
  typedef SceneGraph::OpenGL::DrawOpenGL DrawOpenGL;
  typedef SceneGraph::Interfaces::IContext IContext;

  // Try to get the context.
  IContext::QueryPtr context ( unknown );

  // If we have a valid context then make the draw-method.
  if ( true == context.valid() )
  {
    return DrawOpenGL::RefPtr ( new DrawOpenGL ( context->contextId() ) );
  }

  // Otherwise, we cannot make a draw-method.
  else
  {
    return DrawOpenGL::RefPtr();
  }
}
