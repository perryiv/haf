
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The OpenGL drawing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/Plugins/OpenGL/DrawOpenGL.h"
#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/Nodes/Shapes/Line.h"
#include "SceneGraph/Nodes/Shapes/Geometry.h"
#include "SceneGraph/Plugins/OpenGL/Scope.h"
#include "SceneGraph/State/Attributes/Attributes.h"
#include "SceneGraph/State/Attributes/Textures.h"

#include "Usul/Bits/Bits.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/Reset.h"

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"

#include "GL/glu.h"

#include <list>
#include <vector>

using namespace SceneGraph::OpenGL;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs with file scope.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for helper functions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef boost::function2 < void, GLuint, const boost::any & > UniformSetter;
  typedef std::map < std::string, UniformSetter > UniformSetters;
  typedef Usul::Math::Vec2f Vec2;
  typedef Usul::Math::Vec3f Vec3;
  typedef Usul::Math::Vec4f Vec4;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to set the uniform.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void setUniform1f ( GLint location, const boost::any &value )
  {
    ::glUniform1f ( location, boost::any_cast<float> ( value ) );
  }
  inline void setUniform2f ( GLint location, const boost::any &value )
  {
    const Vec2 v ( boost::any_cast<Vec2> ( value ) );
    ::glUniform2f ( location, v[0], v[1] );
  }
  inline void setUniform3f ( GLint location, const boost::any &value )
  {
    const Vec3 v ( boost::any_cast<Vec3> ( value ) );
    ::glUniform3f ( location, v[0], v[1], v[2] );
  }
  inline void setUniform4f ( GLint location, const boost::any &value )
  {
    const Vec4 v ( boost::any_cast<Vec4> ( value ) );
    ::glUniform4f ( location, v[0], v[1], v[2], v[3] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the map of uniform setters.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T, class F >
  inline void addUniformSetter ( UniformSetters &setters, F f )
  {
    const std::type_info &type ( typeid ( T ) );
    const char *name ( type.name() );
    if ( 0x0 != name )
    {
      setters[name] = boost::bind ( f, _1, _2 );
    }
  }

  inline UniformSetters makeUniformSetters()
  {
    UniformSetters setters;

    Helper::addUniformSetter<float> ( setters, &Helper::setUniform1f );
    Helper::addUniformSetter<Vec2>  ( setters, &Helper::setUniform2f );
    Helper::addUniformSetter<Vec3>  ( setters, &Helper::setUniform3f );
    Helper::addUniformSetter<Vec4>  ( setters, &Helper::setUniform4f );

    return setters;
  }

  const UniformSetters _uniformSetters ( Helper::makeUniformSetters() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the handler for the shader uniform type.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline UniformSetter getUniformSetter ( const boost::any &value )
  {
    const std::type_info &type ( value.type() );
    const char *name ( type.name() );
    if ( 0x0 != name )
    {
      typedef UniformSetters::const_iterator Itr;
      Itr i ( _uniformSetters.find ( std::string ( name ) ) );
      if ( _uniformSetters.end() != i )
      {
        return i->second;
      }
    }

    // If we get to here then we didn't find one.
    return UniformSetter();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for setting shader uniform values.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void setShaderParameter 
    ( GLuint program, const std::string &name, const boost::any &value )
  {
    if ( false == name.empty() )
    {
      const GLint location ( ::glGetUniformLocation ( program, name.c_str() ) );
      UniformSetter setter ( Helper::getUniformSetter ( value ) );
      if ( setter )
      {
        setter ( location, value );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load face enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > Faces;
  Faces faces;
  namespace
  {
    struct InitFaces
    {
      InitFaces()
      {
        USUL_STATIC_ASSERT ( 0 == SceneGraph::State::Attributes::Face::FRONT );
        faces.push_back ( GL_FRONT );
        faces.push_back ( GL_BACK );
        faces.push_back ( GL_FRONT_AND_BACK );
      }
    } initFaces;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load mode enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > PolygonModes;
  PolygonModes polygonModes;
  namespace
  {
    struct InitPolygonModes
    {
      InitPolygonModes()
      {
        typedef SceneGraph::State::Attributes::PolygonMode PolygonMode;
        USUL_STATIC_ASSERT ( 0 == PolygonMode::POINTS );
        polygonModes.push_back ( GL_POINT );
        polygonModes.push_back ( GL_LINE );
        polygonModes.push_back ( GL_FILL );
      }
    } initPolygonModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load shade model enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > ShadeModels;
  ShadeModels shadeModels;
  namespace
  {
    struct InitShadeModels
    {
      InitShadeModels()
      {
        typedef SceneGraph::State::Attributes::ShadeModel ShadeModel;
        USUL_STATIC_ASSERT ( 0 == ShadeModel::FLAT );
        shadeModels.push_back ( GL_FLAT );
        shadeModels.push_back ( GL_SMOOTH );
      }
    } initShadeModels;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load light model enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > LightModels;
  LightModels lightModels;
  namespace
  {
    struct InitLightModels
    {
      InitLightModels()
      {
        typedef SceneGraph::State::Attributes::LightModel LightModel;
        USUL_STATIC_ASSERT ( 0 == LightModel::AMBIENT );
        lightModels.push_back ( GL_LIGHT_MODEL_AMBIENT );
        lightModels.push_back ( GL_LIGHT_MODEL_COLOR_CONTROL );
        lightModels.push_back ( GL_LIGHT_MODEL_LOCAL_VIEWER );
        lightModels.push_back ( GL_LIGHT_MODEL_TWO_SIDE );
      }
    } initLightModels;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load primitive mode enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > PrimitiveModes;
  PrimitiveModes primitiveModes;
  namespace
  {
    struct InitPrimitiveModes
    {
      InitPrimitiveModes()
      {
        USUL_STATIC_ASSERT ( 0 == SceneGraph::Nodes::Shapes::Geometry::POINTS );
        primitiveModes.push_back ( GL_POINTS );
        primitiveModes.push_back ( GL_LINE_STRIP );
        primitiveModes.push_back ( GL_LINE_LOOP );
        primitiveModes.push_back ( GL_LINES );
        primitiveModes.push_back ( GL_TRIANGLE_STRIP );
        primitiveModes.push_back ( GL_TRIANGLE_FAN );
        primitiveModes.push_back ( GL_TRIANGLES );
        primitiveModes.push_back ( GL_QUAD_STRIP );
        primitiveModes.push_back ( GL_QUADS );
        primitiveModes.push_back ( GL_POLYGON );
      }
    } initPrimitiveModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load blending functions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > BlendingFactor;
  BlendingFactor blendingFactor;
  namespace
  {
    struct InitBlendingFactor
    {
      InitBlendingFactor()
      {
        typedef SceneGraph::State::Attributes::Blending Blending;
        USUL_STATIC_ASSERT ( 0 == Blending::ZERO );
        blendingFactor.push_back ( GL_ZERO );
        blendingFactor.push_back ( GL_ONE );
        blendingFactor.push_back ( GL_SRC_COLOR );
        blendingFactor.push_back ( GL_ONE_MINUS_SRC_COLOR );
        blendingFactor.push_back ( GL_DST_COLOR );
        blendingFactor.push_back ( GL_ONE_MINUS_DST_COLOR );
        blendingFactor.push_back ( GL_SRC_ALPHA );
        blendingFactor.push_back ( GL_ONE_MINUS_SRC_ALPHA );
        blendingFactor.push_back ( GL_DST_ALPHA );
        blendingFactor.push_back ( GL_ONE_MINUS_DST_ALPHA );
        blendingFactor.push_back ( GL_CONSTANT_COLOR );
        blendingFactor.push_back ( GL_ONE_MINUS_CONSTANT_COLOR );
        blendingFactor.push_back ( GL_CONSTANT_ALPHA );
        blendingFactor.push_back ( GL_ONE_MINUS_CONSTANT_ALPHA );
        blendingFactor.push_back ( GL_SRC_ALPHA_SATURATE );
      }
    } initBlendingFactor;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load light name enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > LightParameters;
  LightParameters lightParameters;
  namespace
  {
    struct InitLightParameters
    {
      InitLightParameters()
      {
        typedef SceneGraph::State::Attributes::Light Light;
        USUL_STATIC_ASSERT ( 0 == Light::AMBIENT );
        lightParameters.push_back ( GL_AMBIENT );
        lightParameters.push_back ( GL_DIFFUSE );
        lightParameters.push_back ( GL_SPECULAR );
        lightParameters.push_back ( GL_POSITION );
        lightParameters.push_back ( GL_SPOT_DIRECTION );
        lightParameters.push_back ( GL_SPOT_EXPONENT );
        lightParameters.push_back ( GL_SPOT_CUTOFF );
        lightParameters.push_back ( GL_CONSTANT_ATTENUATION );
        lightParameters.push_back ( GL_LINEAR_ATTENUATION );
        lightParameters.push_back ( GL_QUADRATIC_ATTENUATION );
      }
    } initLightParameters;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load enable mode enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef boost::function1 < void, GLenum > EnablerFunction;
  typedef std::pair < EnablerFunction, EnablerFunction > EnablerFunctions;
  typedef std::pair < GLenum, EnablerFunctions > EnablerData;
  typedef std::vector < EnablerData > Enablers;
  Enablers enablers;
  namespace
  {
    struct InitEnablers
    {
      InitEnablers()
      {
        typedef SceneGraph::State::Attributes::Enable Enable;
        USUL_STATIC_ASSERT ( 0 == Enable::ALPHA_TEST );

        // Flags for the server.
        this->_addServerHandlers ( GL_ALPHA_TEST );
        this->_addServerHandlers ( GL_AUTO_NORMAL );
        this->_addServerHandlers ( GL_BLEND );
        this->_addServerHandlers ( GL_CLIP_PLANE0 );
        this->_addServerHandlers ( GL_CLIP_PLANE1 );
        this->_addServerHandlers ( GL_CLIP_PLANE2 );
        this->_addServerHandlers ( GL_CLIP_PLANE3 );
        this->_addServerHandlers ( GL_CLIP_PLANE4 );
        this->_addServerHandlers ( GL_CLIP_PLANE5 );
        this->_addServerHandlers ( GL_COLOR_LOGIC_OP );
        this->_addServerHandlers ( GL_COLOR_MATERIAL );
        this->_addServerHandlers ( GL_COLOR_SUM );
        this->_addServerHandlers ( GL_COLOR_TABLE );
        this->_addServerHandlers ( GL_CONVOLUTION_1D );
        this->_addServerHandlers ( GL_CONVOLUTION_2D );
        this->_addServerHandlers ( GL_CULL_FACE );
        this->_addServerHandlers ( GL_DEPTH_TEST );
        this->_addServerHandlers ( GL_DITHER );
        this->_addServerHandlers ( GL_FOG );
        this->_addServerHandlers ( GL_HISTOGRAM );
        this->_addServerHandlers ( GL_INDEX_LOGIC_OP );
        this->_addServerHandlers ( GL_LIGHT0 );
        this->_addServerHandlers ( GL_LIGHT1 );
        this->_addServerHandlers ( GL_LIGHT2 );
        this->_addServerHandlers ( GL_LIGHT3 );
        this->_addServerHandlers ( GL_LIGHT4 );
        this->_addServerHandlers ( GL_LIGHT5 );
        this->_addServerHandlers ( GL_LIGHT6 );
        this->_addServerHandlers ( GL_LIGHT7 );
        this->_addServerHandlers ( GL_LIGHTING );
        this->_addServerHandlers ( GL_LINE_SMOOTH );
        this->_addServerHandlers ( GL_LINE_STIPPLE );
        this->_addServerHandlers ( GL_MAP1_COLOR_4 );
        this->_addServerHandlers ( GL_MAP1_INDEX );
        this->_addServerHandlers ( GL_MAP1_NORMAL );
        this->_addServerHandlers ( GL_MAP1_TEXTURE_COORD_1 );
        this->_addServerHandlers ( GL_MAP1_TEXTURE_COORD_2 );
        this->_addServerHandlers ( GL_MAP1_TEXTURE_COORD_3 );
        this->_addServerHandlers ( GL_MAP1_TEXTURE_COORD_4 );
        this->_addServerHandlers ( GL_MAP1_VERTEX_3 );
        this->_addServerHandlers ( GL_MAP1_VERTEX_4 );
        this->_addServerHandlers ( GL_MAP2_COLOR_4 );
        this->_addServerHandlers ( GL_MAP2_INDEX );
        this->_addServerHandlers ( GL_MAP2_NORMAL );
        this->_addServerHandlers ( GL_MAP2_TEXTURE_COORD_1 );
        this->_addServerHandlers ( GL_MAP2_TEXTURE_COORD_2 );
        this->_addServerHandlers ( GL_MAP2_TEXTURE_COORD_3 );
        this->_addServerHandlers ( GL_MAP2_TEXTURE_COORD_4 );
        this->_addServerHandlers ( GL_MAP2_VERTEX_3 );
        this->_addServerHandlers ( GL_MAP2_VERTEX_4 );
        this->_addServerHandlers ( GL_MINMAX );
        this->_addServerHandlers ( GL_MULTISAMPLE );
        this->_addServerHandlers ( GL_NORMALIZE );
        this->_addServerHandlers ( GL_POINT_SMOOTH );
        this->_addServerHandlers ( GL_POINT_SPRITE );
        this->_addServerHandlers ( GL_POLYGON_OFFSET_FILL );
        this->_addServerHandlers ( GL_POLYGON_OFFSET_LINE );
        this->_addServerHandlers ( GL_POLYGON_OFFSET_POINT );
        this->_addServerHandlers ( GL_POLYGON_SMOOTH );
        this->_addServerHandlers ( GL_POLYGON_STIPPLE );
        this->_addServerHandlers ( GL_POST_COLOR_MATRIX_COLOR_TABLE );
        this->_addServerHandlers ( GL_POST_CONVOLUTION_COLOR_TABLE );
        this->_addServerHandlers ( GL_RESCALE_NORMAL );
        this->_addServerHandlers ( GL_SAMPLE_ALPHA_TO_COVERAGE );
        this->_addServerHandlers ( GL_SAMPLE_ALPHA_TO_ONE );
        this->_addServerHandlers ( GL_SAMPLE_COVERAGE );
        this->_addServerHandlers ( GL_SEPARABLE_2D );
        this->_addServerHandlers ( GL_SCISSOR_TEST );
        this->_addServerHandlers ( GL_STENCIL_TEST );
        this->_addServerHandlers ( GL_TEXTURE_1D );
        this->_addServerHandlers ( GL_TEXTURE_2D );
        this->_addServerHandlers ( GL_TEXTURE_3D );
        this->_addServerHandlers ( GL_TEXTURE_CUBE_MAP );
        this->_addServerHandlers ( GL_TEXTURE_GEN_Q );
        this->_addServerHandlers ( GL_TEXTURE_GEN_R );
        this->_addServerHandlers ( GL_TEXTURE_GEN_S );
        this->_addServerHandlers ( GL_TEXTURE_GEN_T );
        this->_addServerHandlers ( GL_VERTEX_PROGRAM_POINT_SIZE );
        this->_addServerHandlers ( GL_VERTEX_PROGRAM_TWO_SIDE );

        // Flags for the client.
        this->_addClientHandlers ( GL_COLOR_ARRAY );
        this->_addClientHandlers ( GL_EDGE_FLAG_ARRAY );
        this->_addClientHandlers ( GL_INDEX_ARRAY );
        this->_addClientHandlers ( GL_NORMAL_ARRAY );
        this->_addClientHandlers ( GL_TEXTURE_COORD_ARRAY );
        this->_addClientHandlers ( GL_VERTEX_ARRAY );
      }
    private:
      void _addServerHandlers ( GLenum mode )
      {
        EnablerFunctions fp ( ::glEnable, ::glDisable );
        enablers.push_back ( EnablerData ( mode, fp ) );
      }
      void _addClientHandlers ( GLenum mode )
      {
        EnablerFunctions fp ( ::glEnableClientState, ::glDisableClientState );
        enablers.push_back ( EnablerData ( mode, fp ) );
      }
    } initEnableModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load hint targets.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > HintTargets;
  HintTargets hintTargets;
  namespace
  {
    struct InitHintTargets
    {
      InitHintTargets()
      {
        typedef SceneGraph::State::Attributes::Hint Hint;
        USUL_STATIC_ASSERT ( 0 == Hint::FOG_HINT );
        hintTargets.push_back ( GL_FOG_HINT );
        hintTargets.push_back ( GL_GENERATE_MIPMAP_HINT );
        hintTargets.push_back ( GL_LINE_SMOOTH_HINT );
        hintTargets.push_back ( GL_PERSPECTIVE_CORRECTION_HINT );
        hintTargets.push_back ( GL_POINT_SMOOTH_HINT );
        hintTargets.push_back ( GL_POLYGON_SMOOTH_HINT );
        hintTargets.push_back ( GL_TEXTURE_COMPRESSION_HINT );
        hintTargets.push_back ( GL_FRAGMENT_SHADER_DERIVATIVE_HINT );
      }
    } initHintTargets;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-load hint modes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::vector < GLenum > HintModes;
  HintModes hintModes;
  namespace
  {
    struct InitHintModes
    {
      InitHintModes()
      {
        typedef SceneGraph::State::Attributes::Hint Hint;
        USUL_STATIC_ASSERT ( 0 == Hint::FASTEST );
        hintModes.push_back ( GL_FASTEST );
        hintModes.push_back ( GL_NICEST );
        hintModes.push_back ( GL_DONT_CARE );
      }
    } initHintModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DrawOpenGL::DrawOpenGL ( unsigned long id ) : BaseClass(),
  _id ( id ),
  _currentStateContainer(),
  _currentShader(),
  _currentProgram ( 0 ),
  _programMap(),
  _sharedPrimitives ( new SharedPrimitives() ),
  _currentModelview ( Matrix::getIdentity() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DrawOpenGL::~DrawOpenGL()
{
  USUL_TRY_BLOCK
  {
    _currentStateContainer = StateContainer::RefPtr();
    _currentShader = Shader::RefPtr();
    _currentProgram = 0;
    _programMap.clear();
    _sharedPrimitives = SharedPrimitives::RefPtr();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "4023022349" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset this instance.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::reset()
{
  // No current objects.
  _currentStateContainer = StateContainer::RefPtr();
  _currentShader = Shader::RefPtr();
  _currentProgram = 0;
  _sharedPrimitives = SharedPrimitives::RefPtr ( new SharedPrimitives() );
  _currentModelview = Matrix::getIdentity();

  // Clear the background.
  ::glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
              GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

  // Redirect to base class.
  BaseClass::reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check for additional error information.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkShaderInfoLog ( const std::string &errorId, GLuint shaderId )
  {
    // Get the length of the log.
    int param ( 0 );
    ::glGetShaderiv ( shaderId, GL_INFO_LOG_LENGTH, &param );
    if ( param <= 0 )
      return;

    // Allocate a buffer for the log.
    typedef std::vector<char> Buffer;
    Buffer buffer ( param, '\0' );
    int length ( 0 );
    ::glGetShaderInfoLog ( shaderId, buffer.size() - 1, &length, &buffer[0] );
    if ( ( length > 0 ) && ( length < static_cast<int> ( buffer.size() ) ) )
    {
      std::string message ( buffer.begin(), buffer.begin() + length );
      throw Usul::Exceptions::Error ( errorId, message );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check for additional error information.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkProgramInfoLog ( const std::string &errorId, GLuint programId )
  {
    // Get the length of the log.
    int param ( 0 );
    ::glGetProgramiv ( programId, GL_INFO_LOG_LENGTH, &param );
    if ( param <= 0 )
      return;

    // Allocate a buffer for the log.
    typedef std::vector<char> Buffer;
    Buffer buffer ( param, '\0' );
    int length ( 0 );
    ::glGetProgramInfoLog ( programId, buffer.size() - 1, &length, &buffer[0] );
    if ( ( length > 0 ) && ( length < static_cast<int> ( buffer.size() ) ) )
    {
      std::string message ( buffer.begin(), buffer.begin() + length );
      throw Usul::Exceptions::Error ( errorId, message );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check the shader.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkShaderCompileStatus ( const std::string &errorId, GLuint shaderId )
  {
    // See if it worked.
    int param ( GL_FALSE );
    ::glGetShaderiv ( shaderId, GL_COMPILE_STATUS, &param );
    if ( GL_TRUE != param )
    {
      // Check info log for more details.
      Helper::checkShaderInfoLog ( errorId, shaderId );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check the program.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkProgramLinkStatus ( const std::string &errorId, GLuint programId )
  {
    // See if it worked.
    int param ( GL_FALSE );
    ::glGetProgramiv ( programId, GL_LINK_STATUS, &param );
    if ( GL_TRUE != param )
    {
      // Check info log for more details.
      Helper::checkProgramInfoLog ( errorId, programId );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get or make the program id.
//
///////////////////////////////////////////////////////////////////////////////

GLuint DrawOpenGL::_getProgramId ( Program::RefPtr program )
{
  // Handle trivial case.
  if ( false == program.valid() )
  {
    return 0;
  }

  // Look for the id.
  ProgramMap::const_iterator i ( _programMap.find ( program ) );
  if ( _programMap.end() != i )
  {
    return i->second;
  }

  // If we get to here then it's the first time, so compile and link.

  // Get the code.
  const std::string &vc ( program->vertexCode() );
  const std::string &fc ( program->fragmentCode() );
  if ( true == vc.empty() )
  {
    throw Usul::Exceptions::Error ( "2635238208", Usul::Strings::format
      ( "Empty vertex code in program '", program->name(), "'" ) );
  }
  if ( true == fc.empty() )
  {
    throw Usul::Exceptions::Error ( "1818684420", Usul::Strings::format
      ( "Empty fragment code in program '", program->name(), "'" ) );
  }

  // Get the code as arrays.
  const char *vca ( vc.c_str() );
  const char *fca ( fc.c_str() );
  if ( 0x0 == vca )
  {
    throw Usul::Exceptions::Error ( "2398521629", Usul::Strings::format
      ( "Empty vertex code in program '", program->name(), "'" ) );
  }
  if ( 0x0 == fca )
  {
    throw Usul::Exceptions::Error ( "1359907811", Usul::Strings::format
      ( "Empty fragment code in program '", program->name(), "'" ) );
  }

  // Create the vertex shader.
  GLuint vs ( ::glCreateShader ( GL_VERTEX_SHADER ) );
  ::glShaderSource ( vs, 1, &vca, 0x0 );
  ::glCompileShader ( vs );
  Helper::checkShaderCompileStatus ( "1485954818", vs );

  // Create the fragment shader.
  GLuint fs ( ::glCreateShader ( GL_FRAGMENT_SHADER ) );
  ::glShaderSource ( fs, 1, &fca, 0x0 );
  ::glCompileShader ( fs );
  Helper::checkShaderCompileStatus ( "8330384040", fs );

  // Create the program.
  GLuint id ( ::glCreateProgram() );

  // Attach the shaders to the program.
  ::glAttachShader ( id, vs );
  ::glAttachShader ( id, fs );

  // Link the program.
  ::glLinkProgram ( id );
  Helper::checkProgramLinkStatus ( "5171582200", id );

  // Save the program id in our map.
  _programMap[program] = id;

  // Return the new program id.
  return id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the given shader be OpenGL's current shader.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( Shader &shader )
{
  // Ignore the same shader.
  if ( _currentShader.get() == &shader )
    return;

  // Set new current shader.
  _currentShader = Shader::RefPtr ( &shader );

  // Do nothing with shaders if it's not supported.
  if ( GL_FALSE == GLEE_ARB_shader_objects )
    return;

  // Get the program id.
  GLuint program ( this->_getProgramId ( shader.programGet() ) );

  // If it is a different program then use it. This will switch to fixed 
  // functionality if it's zero.
  if ( _currentProgram != program )
  {
    _currentProgram = program;
    ::glUseProgram ( program );
  }

  // If we are not using fixed functionality then set the shader's parameters.
  if ( 0 != program )
  {
    this->_setShaderParameters ( program );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shader's parameters.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::_setShaderParameters ( GLuint program )
{
  // Handle no shader.
  if ( false == _currentShader.valid() )
    return;

  // Do nothing with OpenGL if it's not supported.
  if ( GL_FALSE == GLEE_ARB_shader_objects )
    return;

  // Get the parameter names.
  Shader::ParamNames names ( _currentShader->paramNames() );
  if ( true == names.empty() )
    return;

  // Loop through all the parameters.
  typedef Shader::ParamNames::const_iterator Itr;
  for ( Itr i = names.begin(); i != names.end(); ++i )
  {
    const std::string &name ( *i );
    const boost::any value ( _currentShader->paramGet ( name ) );
    Helper::setShaderParameter ( program, name, value );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( SceneGraph::Nodes::Shapes::Line &line )
{
  this->_checkContinue();

  typedef SceneGraph::Nodes::Shapes::Line Line;
  const Line::Vector a ( line.startGet() );
  const Line::Vector b ( line.endGet() );

  typedef SceneGraph::OpenGL::Scope::BeginEnd BeginEnd;
  BeginEnd be ( GL_LINES );
  {
    ::glVertex3d ( a[0], a[1], a[2] );
    ::glVertex3d ( b[0], b[1], b[2] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( SceneGraph::Nodes::Shapes::Geometry &geometry )
{
  typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
  typedef Geometry::SharedVertices SharedVertices;
  typedef Geometry::SharedNormals SharedNormals;
  typedef Geometry::SharedTexCoords SharedTexCoords;
  typedef Geometry::SharedColors SharedColors;
  typedef Geometry::Primitives Primitives;
  typedef Geometry::Primitive Primitive;
  typedef Geometry::Indices Indices;
  typedef Geometry::Vertex Vertex;
  typedef Geometry::Normal Normal;
  typedef Geometry::TexCoord TexCoord;
  typedef Geometry::Color Color;

  USUL_ASSERT_SAME_TYPE ( unsigned int, Indices::value_type );
  USUL_ASSERT_SAME_TYPE ( float, Vertex::value_type );
  USUL_ASSERT_SAME_TYPE ( float, Normal::value_type );
  USUL_ASSERT_SAME_TYPE ( float, TexCoord::value_type );
  USUL_ASSERT_SAME_TYPE ( float, Color::value_type );

  this->_checkContinue();

  // Set the vertices.
  {
    const SharedVertices::RefPtr sv ( geometry.verticesGet() );
    if ( true == sv.valid() )
    {
      const SharedVertices::AtomicVector &av ( sv->get() );
      Guard guard ( av.mutex() );
      const SharedVertices::Vector &v ( av.getReference() );
      if ( false == v.empty() )
      {
        ::glVertexPointer ( Vertex::SIZE, GL_FLOAT, 0, &v[0] );
      }
    }
  }

  // Set the normals.
  {
    const SharedNormals::RefPtr sn ( geometry.normalsGet() );
    if ( true == sn.valid() )
    {
      const SharedNormals::AtomicVector &an ( sn->get() );
      Guard guard ( an.mutex() );
      const SharedNormals::Vector &n ( an.getReference() );
      if ( false == n.empty() )
      {
        ::glNormalPointer ( GL_FLOAT, 0, &n[0] );
      }
    }
  }

  // Set the texture coordinates.
  {
    const SharedTexCoords::RefPtr st ( geometry.texCoordsGet() );
    if ( true == st.valid() )
    {
      const SharedTexCoords::AtomicVector &at ( st->get() );
      Guard guard ( at.mutex() );
      const SharedTexCoords::Vector &t ( at.getReference() );
      if ( false == t.empty() )
      {
        ::glTexCoordPointer ( TexCoord::SIZE, GL_FLOAT, 0, &t[0] );
      }
    }
  }

  // Set the colors.
  {
    const SharedColors::RefPtr sc ( geometry.colorsGet() );
    if ( true == sc.valid() )
    {
      const SharedColors::AtomicVector &ac ( sc->get() );
      Guard guard ( ac.mutex() );
      const SharedColors::Vector &c ( ac.getReference() );
      if ( false == c.empty() )
      {
        ::glColorPointer ( Color::SIZE, GL_FLOAT, 0, &c[0] );
      }
    }
  }

  // Loop through the primitives.
  {
    // Initialize.
    SharedPrimitives::RefPtr sp;

    // Get the shared primitives. Sort them if there is a callback.
    Geometry::SortPrimitivesCallback cb ( geometry.sortPrimitivesCallbackGet() );
    if ( cb )
    {
      cb ( _currentModelview, geometry, _sharedPrimitives );
      sp = _sharedPrimitives;
    }
    else
    {
      sp = geometry.primitivesGet();
    }

    // If we have shared primitives...
    if ( true == sp.valid() )
    {
      const SharedPrimitives::AtomicVector &ap ( sp->get() );
      Guard guard ( ap.mutex() );
      const Primitives &ps ( ap.getReference() );
      for ( Primitives::const_iterator i = ps.begin(); i != ps.end(); ++i )
      {
        this->_checkContinue();
        const Primitive &p ( *i );
        const GLenum mode ( Helper::primitiveModes.at ( p.first ) );
        const Indices &indices ( p.second );
        ::glDrawElements ( mode, indices.size(), GL_UNSIGNED_INT, &indices[0] );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Blending &b )
{
  this->_checkContinue();

  const GLenum source ( Helper::blendingFactor.at ( b.source() ) );
  const GLenum destination ( Helper::blendingFactor.at ( b.destination() ) );

  ::glBlendFunc ( source, destination );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Color &c )
{
  this->_checkContinue();
  const SceneGraph::State::Attributes::Color::Vec4 &v ( c.color() );
  ::glColor4d ( v[0], v[1], v[2], v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Enable &enable )
{
  this->_checkContinue();

  const Helper::EnablerData &data ( Helper::enablers.at ( enable.mode() ) );
  const Helper::EnablerFunctions &functions ( data.second );
  const GLenum &state ( data.first );

  if ( enable.state() )
  {
    const Helper::EnablerFunction &enabler ( functions.first );
    enabler ( state );
  }
  else
  {
    const Helper::EnablerFunction &disabler ( functions.second );
    disabler ( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Hint &hint )
{
  this->_checkContinue();

  const GLenum target ( Helper::hintTargets.at ( hint.target() ) );
  const GLenum mode ( Helper::hintModes.at ( hint.mode() ) );

  ::glHint ( target, mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Light &light )
{
  typedef SceneGraph::State::Attributes::Light Light;

  this->_checkContinue();

  const Light::Value &v ( light.value() );
  const GLenum source ( GL_LIGHT0 + light.source() );
  const GLenum parameter ( Helper::lightParameters.at ( light.parameter() ) );

  ::glLightfv ( source, parameter, v.getUnsafePointer() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::Material &m )
{
  typedef SceneGraph::State::Attributes::Material Material;
  typedef Material::Vec4 Vec4;

  this->_checkContinue();

  const Vec4 &a  ( m.ambient()   );
  const Vec4 &d  ( m.diffuse()   );
  const Vec4 &s  ( m.specular()  );
  const Vec4 &e  ( m.emission()  );
  const float sh ( m.shininess() );

  const GLenum face ( Helper::faces.at ( m.face() ) );

  ::glMaterialfv ( face, GL_AMBIENT,   a.getUnsafePointer() );
  ::glMaterialfv ( face, GL_DIFFUSE,   d.getUnsafePointer() );
  ::glMaterialfv ( face, GL_SPECULAR,  s.getUnsafePointer() );
  ::glMaterialfv ( face, GL_EMISSION,  e.getUnsafePointer() );
  ::glMaterialf  ( face, GL_SHININESS, sh      );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::CullFace &c )
{
  this->_checkContinue();
  ::glCullFace ( Helper::faces.at ( c.face() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::LineWidth &w )
{
  this->_checkContinue();
  ::glLineWidth ( w.width() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::PolygonMode &p )
{
  this->_checkContinue();
  const GLenum face ( Helper::faces.at ( p.face() ) );
  const GLenum mode ( Helper::polygonModes.at ( p.mode() ) );
  ::glPolygonMode ( face, mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::ShadeModel &sm )
{
  this->_checkContinue();
  const GLenum model ( Helper::shadeModels.at ( sm.model() ) );
  ::glShadeModel ( model );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( const SceneGraph::State::Attributes::LightModel &lm )
{
  this->_checkContinue();
  const GLenum name ( Helper::lightModels.at ( lm.name() ) );
  ::glLightModelfv ( name, lm.params().getUnsafePointer() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for texture info.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct TextureInfo : public Usul::Base::Referenced
  {
    typedef Usul::Base::Referenced BaseClass;
    USUL_REFERENCED_CLASS ( TextureInfo );

    TextureInfo ( unsigned int contextId, GLuint name ) : 
      BaseClass(), 
      _contextId ( contextId ),
      _name ( name )
    {
    }

    ~TextureInfo()
    {
    }

    GLuint name() const
    {
      return _name;
    }

  private:

    const unsigned int _contextId;
    const GLuint _name;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to determine the texture format.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  GLenum format ( unsigned int numChannels )
  {
    GLenum format ( 0 );
    switch ( numChannels )
    {
    case 1:
      format = GL_LUMINANCE;
      break;
    case 2:
      format = GL_LUMINANCE_ALPHA;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    }
    return format;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( SceneGraph::State::Attributes::Texture2 &t )
{
  typedef SceneGraph::State::Attributes::Texture2 Texture2;
  typedef Texture2::AtomicImagePtr AtomicImagePtr;
  typedef AtomicImagePtr::ValueType ImagePtr;

  this->_checkContinue();

  // Look up the texture name.
  GLuint name ( 0 );
  bool valid ( false );
  {
    typedef Helper::TextureInfo TI;
    TI::RefPtr info ( dynamic_cast < TI * > ( t.rendererDataGet ( _id ).get() ) );
    if ( true == info.valid() )
    {
      name = info->name();
      valid = true;
    }
  }

  // If we didn't find a texture name, see if there is an image.
  if ( false == valid )
  {
    AtomicImagePtr &atomicImage ( t.imageGet() );
    Guard guard ( atomicImage.mutex() );
    ImagePtr image ( atomicImage.getReference() );
    if ( false == image.valid() )
      return;

    // Figure out the format.
    GLenum format ( Helper::format ( image->numChannels() ) );
    if ( 0 == format )
      return;

    // Make the new texture name and save it.
    ::glGenTextures ( 1, &name );
    valid = true;
    t.rendererDataSet ( _id, new Helper::TextureInfo ( _id, name ) );

    // Make this new texture current.
    ::glBindTexture ( GL_TEXTURE_2D, name );

    // Build the image pyramids.
    const unsigned char *bytes ( image->bytes() );
    ::gluBuild2DMipmaps ( GL_TEXTURE_2D, image->numChannels(), image->width(), 
      image->height(), format, GL_UNSIGNED_BYTE, bytes );
  }

  // Otherwise, we have a texture name.
  else
  {
    // Make this texture current.
    ::glBindTexture ( GL_TEXTURE_2D, name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the attribute. Needed to compile.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::visit ( SceneGraph::State::Container &c )
{
  BaseClass::visit ( c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the elements.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::draw()
{
  namespace Face = SceneGraph::State::Attributes::Face;

  // Enable these modes.
  typedef SceneGraph::State::Container StateContainer;
  typedef SceneGraph::State::Attributes::Enable Enable;
  StateContainer::RefPtr state ( new StateContainer );
  state->add ( Enable::RefPtr ( new Enable ( Enable::DEPTH_TEST, true ) ) );
  state->add ( Enable::RefPtr ( new Enable ( Enable::LIGHTING, true ) ) );
  state->add ( Enable::RefPtr ( new Enable ( Enable::LIGHT0, true ) ) );
  this->visit ( *state );

  // Set viewport.
  const Viewport vp ( this->viewportGet() );
  ::glViewport ( 
    static_cast < GLint   > ( vp[0] ), 
    static_cast < GLint   > ( vp[1] ), 
    static_cast < GLsizei > ( vp[2] ), 
    static_cast < GLsizei > ( vp[3] ) );

  // Set projection matrix.
  const Matrix p ( this->projectionMatrixGet() );
  ::glMatrixMode ( GL_PROJECTION );
  ::glLoadMatrixd ( p.get() );

  // Switch back to model-view and load identity.
  ::glMatrixMode ( GL_MODELVIEW );
  ::glLoadMatrixd ( Matrix::getIdentity().get() );

  // Handle no draw lists.
  DrawLists::RefPtr dl ( this->_drawListsGet() );
  if ( false == dl.valid() )
    return;

  // Get a list of the keys.
  typedef std::list < int > Keys;
  Keys keys;
  dl->keys ( keys );

  // This will hold the elements to draw.
  ElementList el;

  // Always push and pop the attributes.
  {
    typedef SceneGraph::OpenGL::Scope::ClientAttributes ClientAttributes;
    typedef SceneGraph::OpenGL::Scope::ServerAttributes ServerAttributes;
    ClientAttributes scopedClientAttributes ( GL_CLIENT_ALL_ATTRIB_BITS );
    ServerAttributes scopedServerAttributes ( GL_ALL_ATTRIB_BITS );

    // Loop through the draw-lists.
    for ( Keys::const_iterator i = keys.begin(); i != keys.end(); ++i )
    {
      // Copy the element pointers.
      const Keys::value_type key ( *i );
      el.clear();
      el.reserve ( dl->numElements ( key ) );
      dl->elements ( key, el );

      // Sort the elements.
      this->_sortElements ( el );

      // Draw the elements.
      this->_drawElements ( el );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicte for sorting the elements.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct ElementSortingPredicate
  {
    typedef SceneGraph::Draw::Element Element;
    typedef SceneGraph::Shaders::Shader Shader;
    typedef SceneGraph::Shaders::Program Program;
    typedef SceneGraph::State::Container StateContainer;

    ElementSortingPredicate(){}

    bool operator () ( Element::RefPtr &le, 
                       Element::RefPtr &re ) const
    {
      // Compare importance.
      const int li ( -1 * le->importance() );
      const int ri ( -1 * re->importance() );
      if ( li < ri )
      {
        return true;
      }
      else if ( li > ri )
      {
        return false;
      }

      // Compare state-containers.
      StateContainer::RefPtr ls ( le->stateContainer() );
      StateContainer::RefPtr rs ( re->stateContainer() );
      if ( ls.get() < rs.get() )
      {
        return true;
      }
      else if ( ls.get() > rs.get() )
      {
        return false;
      }

      // Left is not less than right because they are equal.
      return false;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sort the elements.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::_sortElements ( ElementList &el )
{
  typedef ElementList::iterator Itr;
  typedef ElementList::value_type ElementPtr;

  std::sort ( el.begin(), el.end(), Helper::ElementSortingPredicate() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the elements.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::_drawElements ( ElementList &el )
{
  typedef ElementList::iterator Itr;
  typedef ElementList::value_type ElementPtr;

  for ( Itr i = el.begin(); i != el.end(); ++i )
  {
    ElementPtr &e ( *i );
    if ( true == e.valid() )
    {
      this->_drawElement ( *e );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the element.
//
///////////////////////////////////////////////////////////////////////////////

void DrawOpenGL::_drawElement ( Element &e )
{
  typedef Element::Shape Shape;
  typedef Element::Matrix Matrix;

  // Load the matrix.
  const Matrix n ( this->navigationMatrixGet() );
  _currentModelview = n * e.matrix();
  ::glLoadMatrixd ( _currentModelview.get() );

  // If the state-container is different...
  StateContainer::RefPtr stateContainer ( e.stateContainer() );
  if ( _currentStateContainer != stateContainer )
  {
    // Clear the state.
    ::glPopAttrib();
    ::glPopClientAttrib();
    ::glPushAttrib ( GL_ALL_ATTRIB_BITS );
    ::glPushClientAttrib ( GL_CLIENT_ALL_ATTRIB_BITS );

    // Clear the shader.
    if ( GL_TRUE == GLEE_ARB_shader_objects )
    {
      _currentShader = Shader::RefPtr();
      _currentProgram = 0;
      ::glUseProgram ( 0 );
    }

    // Assign new current.
    _currentStateContainer == stateContainer;

    // Visit the state-container if it's not null.
    if ( true == stateContainer.valid() )
    {
      stateContainer->accept ( *this );
    }
  }

  Shape::RefPtr shape ( e.shape() );
  if ( true == shape.valid() )
  {
    shape->accept ( *this );
  }
}
