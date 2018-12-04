
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The parser class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning ( disable : 4503 ) // Decorated name length exceeded.
#endif

#include "SceneGraph/Plugins/IO/HSG/Parser.h"
#include "SceneGraph/Builders/Builders.h"
#include "SceneGraph/Nodes/Groups/Transform.h"
#include "SceneGraph/Nodes/Shapes/Geometry.h"
#include "SceneGraph/Nodes/Shapes/Line.h"
#include "SceneGraph/Shaders/Manager.h"
#include "SceneGraph/State/Attributes/Attributes.h"

#include "Tree/Find.h"
#include "Tree/Interfaces/IReadXML.h"

#include "Usul/Convert/Matrix44.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Convert/Vector3.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/IO/TextReader.h"
#include "Usul/Math/Constants.h"
#include "Usul/Plugins/Manager.h"
#include "Usul/Strings/Format.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem.hpp"

#include <fstream>
#include <stdexcept>

using namespace SceneGraph::IO::HSG;


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef SceneGraph::Nodes::Groups::Group Group;
typedef SceneGraph::Nodes::Groups::Transform Transform;
typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
typedef SceneGraph::Nodes::Shapes::Line Line;
typedef SceneGraph::Nodes::Shapes::Shape Shape;
typedef SceneGraph::Shaders::Shader Shader;
typedef SceneGraph::Shaders::Manager ShaderManager;
typedef SceneGraph::State::Container StateContainer;
typedef SceneGraph::State::Attributes::Light Light;
typedef SceneGraph::State::Attributes::Blending Blending;


///////////////////////////////////////////////////////////////////////////////
//
//  Definitions for reading a vector of vectors.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::vector < Usul::Math::Vec2f > VectorVec2f;
typedef std::vector < Usul::Math::Vec3f > VectorVec3f;
typedef std::vector < Usul::Math::Vec4f > VectorVec4f;
typedef std::vector < unsigned int >      VectorUInt;
USUL_IO_TEXT_DEFINE_READER_TYPE_SEQUENCE ( VectorVec2f );
USUL_IO_TEXT_DEFINE_READER_TYPE_SEQUENCE ( VectorVec3f );
USUL_IO_TEXT_DEFINE_READER_TYPE_SEQUENCE ( VectorVec4f );
USUL_IO_TEXT_DEFINE_READER_TYPE_SEQUENCE ( VectorUInt  );


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for face enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    namespace Face = SceneGraph::State::Attributes::Face;

    template <> struct Type < std::string, Face::Enum >
    {
      typedef Type < std::string, Face::Enum > ThisType;
      static void convert ( const std::string &from, Face::Enum &to )
      {
        std::string copy ( from );
        std::transform ( copy.begin(), copy.end(), copy.begin(), ::toupper );
        if ( "FRONT" == copy )
        {
          to = Face::FRONT;
        }
        else if ( "BACK" == copy )
        {
          to = Face::BACK;
        }
        else
        {
          to = Face::BOTH;
        }
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for polygon mode enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    typedef SceneGraph::State::Attributes::PolygonMode PolygonMode;
    template <> struct Type < std::string, PolygonMode::Mode >
    {
      typedef Type < std::string, PolygonMode::Mode > ThisType;
      static void convert ( const std::string &from, PolygonMode::Mode &to )
      {
        std::string copy ( from );
        std::transform ( copy.begin(), copy.end(), copy.begin(), ::toupper );
        if ( "POINTS" == copy )
        {
          to = PolygonMode::POINTS;
        }
        else if ( "LINES" == copy )
        {
          to = PolygonMode::LINES;
        }
        else
        {
          to = PolygonMode::FILLED;
        }
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for shade model enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    typedef SceneGraph::State::Attributes::ShadeModel ShadeModel;
    template <> struct Type < std::string, ShadeModel::Model >
    {
      typedef Type < std::string, ShadeModel::Model > ThisType;
      static void convert ( const std::string &from, ShadeModel::Model &to )
      {
        std::string copy ( from );
        std::transform ( copy.begin(), copy.end(), copy.begin(), ::toupper );
        if ( "FLAT" == copy )
        {
          to = ShadeModel::FLAT;
        }
        else
        {
          to = ShadeModel::SMOOTH;
        }
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for light model enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    typedef SceneGraph::State::Attributes::LightModel LightModel;
    template <> struct Type < std::string, LightModel::Name >
    {
      typedef Type < std::string, LightModel::Name > ThisType;
      static void convert ( const std::string &from, LightModel::Name &to )
      {
        std::string copy ( from );
        std::transform ( copy.begin(), copy.end(), copy.begin(), ::toupper );
        if ( "AMBIENT" == copy )
        {
          to = LightModel::AMBIENT;
        }
        else if ( "COLOR_CONTROL" == copy )
        {
          to = LightModel::COLOR_CONTROL;
        }
        else if ( "LOCAL_VIEWER" == copy )
        {
          to = LightModel::LOCAL_VIEWER;
        }
        else if ( "TWO_SIDE" == copy )
        {
          to = LightModel::TWO_SIDE;
        }
        else
        {
          to = LightModel::TWO_SIDE;
        }
      }
      static LightModel::Name convert ( const std::string &from )
      {
        LightModel::Name to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for light parameter enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < std::string, Light::Parameter >
    {
      typedef Type < std::string, Light::Parameter > ThisType;
      static void convert ( const std::string &from, Light::Parameter &to )
      {
        typedef Usul::Convert::Type<std::string,unsigned int> Converter;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Map used to convert a string to a primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::map < std::string, Geometry::Mode > GeometryModes;
  GeometryModes geometryModes;
  namespace 
  {
    struct InitGeometryModes
    {
      InitGeometryModes()
      {
        geometryModes["points"]         = Geometry::POINTS;
        geometryModes["line_strip"]     = Geometry::LINE_STRIP;
        geometryModes["line_loop"]      = Geometry::LINE_LOOP;
        geometryModes["lines"]          = Geometry::LINES;
        geometryModes["triangle_strip"] = Geometry::TRIANGLE_STRIP;
        geometryModes["triangle_fan"]   = Geometry::TRIANGLE_FAN;
        geometryModes["triangles"]      = Geometry::TRIANGLES;
        geometryModes["quad_strip"]     = Geometry::QUAD_STRIP;
        geometryModes["quads"]          = Geometry::QUADS;
        geometryModes["polygon"]        = Geometry::POLYGON;
        geometryModes["invalid_mode"]   = Geometry::INVALID_MODE;
      }
    } initGeometryModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Map used to convert a string to a light parameter.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::map < std::string, Light::Parameter > LightParameters;
  LightParameters lightParameters;
  namespace 
  {
    struct InitLightParameters
    {
      InitLightParameters()
      {
        lightParameters["ambient"]               = Light::AMBIENT;
        lightParameters["diffuse"]               = Light::DIFFUSE;
        lightParameters["specular"]              = Light::SPECULAR;
        lightParameters["position"]              = Light::POSITION;
        lightParameters["spot_direction"]        = Light::SPOT_DIRECTION;
        lightParameters["spot_exponent"]         = Light::SPOT_EXPONENT;
        lightParameters["spot_cutoff"]           = Light::SPOT_CUTOFF;
        lightParameters["constant_attenuation"]  = Light::CONSTANT_ATTENUATION;
        lightParameters["linear_attenuation"]    = Light::LINEAR_ATTENUATION;
        lightParameters["quadratic_attenuation"] = Light::QUADRATIC_ATTENUATION;
      }
    } initLightParameters;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Map used to convert a string to a blending factor.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef std::map < std::string, Blending::Factor > BlendingFactors;
  BlendingFactors blendingFactors;
  namespace 
  {
    struct InitBlendingFactors
    {
      InitBlendingFactors()
      {
        blendingFactors["zero"]                     = Blending::ZERO;
        blendingFactors["one"]                      = Blending::ONE;
        blendingFactors["src_color"]                = Blending::SRC_COLOR;
        blendingFactors["one_minus_src_color"]      = Blending::ONE_MINUS_SRC_COLOR;
        blendingFactors["dst_color"]                = Blending::DST_COLOR;
        blendingFactors["one_minus_dst_color"]      = Blending::ONE_MINUS_DST_COLOR;
        blendingFactors["src_alpha"]                = Blending::SRC_ALPHA;
        blendingFactors["one_minus_src_alpha"]      = Blending::ONE_MINUS_SRC_ALPHA;
        blendingFactors["dst_alpha"]                = Blending::DST_ALPHA;
        blendingFactors["one_minus_dst_alpha"]      = Blending::ONE_MINUS_DST_ALPHA;
        blendingFactors["constant_color"]           = Blending::CONSTANT_COLOR;
        blendingFactors["one_minus_constant_color"] = Blending::ONE_MINUS_CONSTANT_COLOR;
        blendingFactors["constant_alpha"]           = Blending::CONSTANT_ALPHA;
        blendingFactors["one_minus_constant_alpha"] = Blending::ONE_MINUS_CONSTANT_ALPHA;
        blendingFactors["src_alpha_saturate"]       = Blending::SRC_ALPHA_SATURATE;
      }
    } initBlendingFactors;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Map used to convert a string to an enable mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  typedef SceneGraph::State::Attributes::Enable Enable;
  typedef std::map < std::string, Enable::Mode > EnableModes;
  EnableModes enableModes;
  namespace 
  {
    struct InitEnableModes
    {
      InitEnableModes()
      {
        // Server modes.
        enableModes["alpha_test"] = Enable::ALPHA_TEST;
        enableModes["auto_normal"] = Enable::AUTO_NORMAL;
        enableModes["blend"] = Enable::BLEND;
        enableModes["clip_plane0"] = Enable::CLIP_PLANE0;
        enableModes["clip_plane1"] = Enable::CLIP_PLANE1;
        enableModes["clip_plane2"] = Enable::CLIP_PLANE2;
        enableModes["clip_plane3"] = Enable::CLIP_PLANE3;
        enableModes["clip_plane4"] = Enable::CLIP_PLANE4;
        enableModes["clip_plane5"] = Enable::CLIP_PLANE5;
        enableModes["color_logic_op"] = Enable::COLOR_LOGIC_OP;
        enableModes["color_material"] = Enable::COLOR_MATERIAL;
        enableModes["color_sum"] = Enable::COLOR_SUM;
        enableModes["color_table"] = Enable::COLOR_TABLE;
        enableModes["convolution_1d"] = Enable::CONVOLUTION_1D;
        enableModes["convolution_2d"] = Enable::CONVOLUTION_2D;
        enableModes["cull_face"] = Enable::CULL_FACE;
        enableModes["depth_test"] = Enable::DEPTH_TEST;
        enableModes["dither"] = Enable::DITHER;
        enableModes["fog"] = Enable::FOG;
        enableModes["histogram"] = Enable::HISTOGRAM;
        enableModes["index_logic_op"] = Enable::INDEX_LOGIC_OP;
        enableModes["light0"] = Enable::LIGHT0;
        enableModes["light1"] = Enable::LIGHT1;
        enableModes["light2"] = Enable::LIGHT2;
        enableModes["light3"] = Enable::LIGHT3;
        enableModes["light4"] = Enable::LIGHT4;
        enableModes["light5"] = Enable::LIGHT5;
        enableModes["light6"] = Enable::LIGHT6;
        enableModes["light7"] = Enable::LIGHT7;
        enableModes["lighting"] = Enable::LIGHTING;
        enableModes["line_smooth"] = Enable::LINE_SMOOTH;
        enableModes["line_stipple"] = Enable::LINE_STIPPLE;
        enableModes["map1_color_4"] = Enable::MAP1_COLOR_4;
        enableModes["map1_index"] = Enable::MAP1_INDEX;
        enableModes["map1_normal"] = Enable::MAP1_NORMAL;
        enableModes["map1_texture_coord_1"] = Enable::MAP1_TEXTURE_COORD_1;
        enableModes["map1_texture_coord_2"] = Enable::MAP1_TEXTURE_COORD_2;
        enableModes["map1_texture_coord_3"] = Enable::MAP1_TEXTURE_COORD_3;
        enableModes["map1_texture_coord_4"] = Enable::MAP1_TEXTURE_COORD_4;
        enableModes["map1_vertex_3"] = Enable::MAP1_VERTEX_3;
        enableModes["map1_vertex_4"] = Enable::MAP1_VERTEX_4;
        enableModes["map2_color_4"] = Enable::MAP2_COLOR_4;
        enableModes["map2_index"] = Enable::MAP2_INDEX;
        enableModes["map2_normal"] = Enable::MAP2_NORMAL;
        enableModes["map2_texture_coord_1"] = Enable::MAP2_TEXTURE_COORD_1;
        enableModes["map2_texture_coord_2"] = Enable::MAP2_TEXTURE_COORD_2;
        enableModes["map2_texture_coord_3"] = Enable::MAP2_TEXTURE_COORD_3;
        enableModes["map2_texture_coord_4"] = Enable::MAP2_TEXTURE_COORD_4;
        enableModes["map2_vertex_3"] = Enable::MAP2_VERTEX_3;
        enableModes["map2_vertex_4"] = Enable::MAP2_VERTEX_4;
        enableModes["minmax"] = Enable::MINMAX;
        enableModes["multisample"] = Enable::MULTISAMPLE;
        enableModes["normalize"] = Enable::NORMALIZE;
        enableModes["point_smooth"] = Enable::POINT_SMOOTH;
        enableModes["point_sprite"] = Enable::POINT_SPRITE;
        enableModes["polygon_offset_fill"] = Enable::POLYGON_OFFSET_FILL;
        enableModes["polygon_offset_line"] = Enable::POLYGON_OFFSET_LINE;
        enableModes["polygon_offset_point"] = Enable::POLYGON_OFFSET_POINT;
        enableModes["polygon_smooth"] = Enable::POLYGON_SMOOTH;
        enableModes["polygon_stipple"] = Enable::POLYGON_STIPPLE;
        enableModes["post_color_matrix_color_table"] = Enable::POST_COLOR_MATRIX_COLOR_TABLE;
        enableModes["post_convolution_color_table"] = Enable::POST_CONVOLUTION_COLOR_TABLE;
        enableModes["rescale_normal"] = Enable::RESCALE_NORMAL;
        enableModes["sample_alpha_to_coverage"] = Enable::SAMPLE_ALPHA_TO_COVERAGE;
        enableModes["sample_alpha_to_one"] = Enable::SAMPLE_ALPHA_TO_ONE;
        enableModes["sample_coverage"] = Enable::SAMPLE_COVERAGE;
        enableModes["separable_2d"] = Enable::SEPARABLE_2D;
        enableModes["scissor_test"] = Enable::SCISSOR_TEST;
        enableModes["stencil_test"] = Enable::STENCIL_TEST;
        enableModes["texture_1d"] = Enable::TEXTURE_1D;
        enableModes["texture_2d"] = Enable::TEXTURE_2D;
        enableModes["texture_3d"] = Enable::TEXTURE_3D;
        enableModes["texture_cube_map"] = Enable::TEXTURE_CUBE_MAP;
        enableModes["texture_gen_q"] = Enable::TEXTURE_GEN_Q;
        enableModes["texture_gen_r"] = Enable::TEXTURE_GEN_R;
        enableModes["texture_gen_s"] = Enable::TEXTURE_GEN_S;
        enableModes["texture_gen_t"] = Enable::TEXTURE_GEN_T;
        enableModes["vertex_program_point_size"] = Enable::VERTEX_PROGRAM_POINT_SIZE;
        enableModes["vertex_program_two_side"] = Enable::VERTEX_PROGRAM_TWO_SIDE;

        // Client modes.
        enableModes["color_array"] = Enable::COLOR_ARRAY;
        enableModes["edge_flag_array"] = Enable::EDGE_FLAG_ARRAY;
        enableModes["index_array"] = Enable::INDEX_ARRAY;
        enableModes["normal_array"] = Enable::NORMAL_ARRAY;
        enableModes["texture_coord_array"] = Enable::TEXTURE_COORD_ARRAY;
        enableModes["vertex_array"] = Enable::VERTEX_ARRAY;
      }
    } initEnableModes;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser() : 
  _threadCheck(),
  _handlers(),
  _instances()
{
  _handlers["group"]     = boost::bind ( &Parser::_buildGroup,     this, _1 );
  _handlers["transform"] = boost::bind ( &Parser::_buildTransform, this, _1 );
  _handlers["geometry"]  = boost::bind ( &Parser::_buildGeometry,  this, _1 );
  _handlers["line"]      = boost::bind ( &Parser::_buildLine,      this, _1 );
  _handlers["sphere"]    = boost::bind ( &Parser::_buildSphere,    this, _1 );
  _handlers["instance"]  = boost::bind ( &Parser::_buildInstance,  this, _1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::~Parser()
{
  USUL_TRY_BLOCK
  {
    _handlers.clear();
    _instances.clear();
  }
  USUL_DEFINE_CATCH_BLOCKS ( "1034159939" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for registering an instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class NodeType > inline void registerInstance ( 
    const Parser::TreeNode &treeNode, 
    NodeType &node, 
    Parser::Instances &instances )
  {
    // Is it named?
    const std::string name ( treeNode.attribute ( "name" ) );
    if ( false == name.empty() )
    {
      instances[name] = SceneGraph::Nodes::Node::RefPtr ( &node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the tree.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Parser::SceneNode::RefPtr traverse
    ( const Parser::Handlers &handlers, const Parser::TreeNode &treeNode )
  {
    typedef Parser::SceneNode SceneNode;
    typedef Parser::Handlers Handlers;

    // Look for the handler.
    const std::string name ( treeNode.name() );
    Handlers::const_iterator i ( handlers.find ( name ) );
    if ( handlers.end() == i )
      return SceneNode::RefPtr();

    // Get the handler.
    Handlers::mapped_type function ( i->second );
    if ( !function )
      return SceneNode::RefPtr();

    // Call the handler.
    SceneNode::RefPtr node ( function ( treeNode ) );

    // Return the new node.
    return node;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the group.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class GroupType, class InstancesType > 
  inline void buildGroup ( 
    const Parser::Handlers &handlers, 
    const Parser::TreeNode &treeNode,
    InstancesType &instances,
    GroupType &group )
  {
    typedef Parser::SceneNode SceneNode;
    typedef Parser::TreeNode TreeNode;
    typedef TreeNode::Children::const_iterator Itr;

    Helper::registerInstance ( treeNode, group, instances );

    const TreeNode::Children c ( treeNode.children() );
    for ( Itr i = c.begin(); i != c.end(); ++i )
    {
      const TreeNode::RefPtr child ( *i );
      if ( true == child.valid() )
      {
        SceneNode::RefPtr node ( Helper::traverse ( handlers, *child ) );
        if ( true == node.valid() )
        {
          group.append ( node );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the property.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class PropertyType >
  void setProperty ( 
    const Parser::TreeNode &tn, 
    const std::string &name, 
    PropertyType &p )
  {
    typedef Parser::TreeNode TreeNode;

    TreeNode::RefPtr pn = Tree::Algorithms::findFirst 
      ( tn, std::string ( name ), false );
    if ( true == pn.valid() )
    {
      typedef Usul::Convert::Type<std::string,PropertyType> Converter;
      Converter::convert ( pn->value(), p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the cull face.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setCullFace ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::CullFace CullFace;
    namespace Face = SceneGraph::State::Attributes::Face;

    // Look for the cull face.
    TreeNode::RefPtr pmn ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "cull_face" ), false ) );
    if ( false == pmn.valid() )
      return;

    // Make default cull face properties.
    Face::Enum face ( Face::BACK );

    // Look for the properties.
    Helper::setProperty ( *pmn, "face", face );

    // Add a new cull face to the shape's state-container.
    CullFace::RefPtr cullFace ( new CullFace ( face ) );
    shape.stateContainer()->add ( cullFace );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setPolygonMode ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::PolygonMode PolygonMode;
    namespace Face = SceneGraph::State::Attributes::Face;

    // Look for the polygon mode.
    TreeNode::RefPtr pmn ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "polygon_mode" ), false ) );
    if ( false == pmn.valid() )
      return;

    // Make default polygon mode properties.
    Face::Enum face ( Face::BACK );
    PolygonMode::Mode mode ( PolygonMode::FILLED );

    // Look for the properties.
    Helper::setProperty ( *pmn, "face", face );
    Helper::setProperty ( *pmn, "mode", mode );

    // Add a new polygon mode to the shape's state-container.
    PolygonMode::RefPtr polygonMode ( new PolygonMode ( face, mode ) );
    shape.stateContainer()->add ( polygonMode );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the shade model.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setShadeModel ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::ShadeModel ShadeModel;

    // Look for the shade model.
    TreeNode::RefPtr smn ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "shade_model" ), false ) );
    if ( false == smn.valid() )
      return;

    // Make default shade model.
    ShadeModel::Model model ( ShadeModel::SMOOTH );

    // Look for the properties.
    Helper::setProperty ( *smn, "model", model );

    // Add a new shade model to the shape's state-container.
    ShadeModel::RefPtr shadeModel ( new ShadeModel ( model ) );
    shape.stateContainer()->add ( shadeModel );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the blending function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setBlending ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::Blending Blending;

    // Look for the blending function.
    TreeNode::RefPtr bf ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "blending" ), false ) );
    if ( false == bf.valid() )
      return;

    // Make default blending function.
    Blending::Factor source ( Blending::ONE );
    Blending::Factor destination ( Blending::ZERO );

    // Look for the source factor.
    TreeNode::RefPtr sf ( Tree::Algorithms::findFirst 
      ( *bf, std::string ( "source" ), false ) );
    if ( true == sf.valid() )
    {
      const std::string value ( boost::algorithm::trim_copy ( sf->value() ) );
      source = Helper::blendingFactors[value];
    }

    // Look for the destination factor.
    TreeNode::RefPtr df ( Tree::Algorithms::findFirst 
      ( *bf, std::string ( "destination" ), false ) );
    if ( true == df.valid() )
    {
      const std::string value ( boost::algorithm::trim_copy ( df->value() ) );
      destination = Helper::blendingFactors[value];
    }

    // Add a new blending function to the shape's state-container.
    Blending::RefPtr blending ( new Blending ( source, destination ) );
    shape.stateContainer()->add ( blending );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to enable or disable the mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setEnable ( const Parser::TreeNode &tn, Shape &shape, bool state )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::Enable Enable;
    typedef TreeNode::Children TreeChildren;
    typedef TreeChildren::const_iterator TreeItr;
    typedef Helper::EnableModes::const_iterator ModesItr;

    // Look for all the elements.
    TreeNode::Children tc ( Tree::Algorithms::findAll 
      ( tn, std::string ( state ? "enable" : "disable" ), false ) );

    // Loop through the children.
    for ( TreeItr i = tc.begin(); i != tc.end(); ++i )
    {
      const TreeNode::RefPtr &child ( *i );
      if ( true == child.valid() )
      {
        // Look for the mode.
        ModesItr mi ( Helper::enableModes.find ( child->value() ) );
        if ( Helper::enableModes.end() != mi )
        {
          Enable::RefPtr enable ( new Enable ( mi->second, state ) );
          shape.stateContainer()->add ( enable );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the light.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setLight ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef TreeNode::Children TreeChildren;
    typedef TreeChildren::const_iterator TreeItr;
    typedef Helper::LightParameters::const_iterator LightParametersItr;
    typedef Usul::Convert::Type<std::string,unsigned int> ToSource;

    // Look for the light.
    TreeNode::RefPtr ln ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "light" ), false ) );
    if ( false == ln.valid() )
      return;

    // Look for the source.
    TreeNode::RefPtr ls ( Tree::Algorithms::findFirst
      ( *ln, std::string ( "source" ), false ) );
    if ( false == ls.valid() )
      return;

    // Convert the source.
    const unsigned int source ( ToSource::convert 
      ( boost::algorithm::trim_copy ( ls->value() ) ) );

    // Loop through the children.
    const TreeChildren lc ( ln->children() );
    for ( TreeItr i = lc.begin(); i != lc.end(); ++i )
    {
      const TreeNode::RefPtr &c ( *i );
      if ( true == c.valid() )
      {
        // Look for the parameter.
        const std::string param ( boost::algorithm::trim_copy ( c->name() ) );
        LightParametersItr pi ( Helper::lightParameters.find ( param ) );
        if ( Helper::lightParameters.end() != pi )
        {
          // Split the value.
          typedef std::list < std::string > ValueList;
          const std::string value ( boost::algorithm::trim_copy ( c->value() ) );
          ValueList vl;
          boost::algorithm::split ( vl, value, 
            std::bind1st ( std::equal_to<char>(), ' ' ) );

          // Initialize the light pointer.
          Light::RefPtr light;

          // How many numbers in the value?
          if ( 1 == vl.size() )
          {
            typedef Usul::Convert::Type<std::string,float> ToValue;
            const float lv ( ToValue::convert ( value ) );
            light = Light::RefPtr ( new Light ( source, pi->second, lv ) );
          }
          else if ( 3 == vl.size() )
          {
            typedef Usul::Math::Vec3f Vec3f;
            typedef Usul::Convert::Type<std::string,Vec3f> ToValue;
            const Vec3f lv ( ToValue::convert ( value ) );
            light = Light::RefPtr ( new Light ( source, pi->second, lv ) );
          }
          else if ( 4 == vl.size() )
          {
            typedef Usul::Convert::Type<std::string,Light::Value> ToValue;
            const Light::Value lv ( ToValue::convert ( value ) );
            light = Light::RefPtr ( new Light ( source, pi->second, lv ) );
          }

          // If we set the light then add it to the state.
          if ( true == light.valid() )
          {
            shape.stateContainer()->add ( light );
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the light model.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setLightModel ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::LightModel LightModel;

    // Look for the light model.
    TreeNode::RefPtr smn ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "light_model" ), false ) );
    if ( false == smn.valid() )
      return;

    // Is there a "name" element?
    TreeNode::RefPtr nn ( Tree::Algorithms::findFirst
      ( *smn, std::string ( "name" ), false ) );
    if ( false == nn.valid() )
      return;

    // Is there a "params" element?
    TreeNode::RefPtr pn ( Tree::Algorithms::findFirst
      ( *smn, std::string ( "params" ), false ) );
    if ( false == pn.valid() )
      return;

    // Get the name.
    const std::string name ( boost::algorithm::trim_copy ( pn->value() ) );
    if ( true == name.empty() )
      return;

    // Get the parameters.
    const std::string value ( boost::algorithm::trim_copy ( pn->value() ) );
    if ( true == value.empty() )
      return;

    // Convert the name.
    typedef Usul::Convert::Type<std::string,LightModel::Name> NameConverter;
    LightModel::Name lmn ( NameConverter::convert ( name ) );

    // Split the params.
    typedef std::list < std::string > ParamList;
    ParamList pl;
    boost::algorithm::split ( pl, value, 
      std::bind1st ( std::equal_to<char>(), ' ' ) );

    // Initialize the light model pointer.
    LightModel::RefPtr lightModel;

    // How many parameters are there?
    if ( 1 == pl.size() )
    {
      typedef Usul::Convert::Type<std::string,bool> ParamsConverter;
      bool params ( ParamsConverter::convert ( value ) );
      lightModel = LightModel::RefPtr ( new LightModel ( lmn, params ) );
    }
    else if ( 4 == pl.size() )
    {
      typedef Usul::Convert::Type<std::string,LightModel::Params> ParamsConverter;
      LightModel::Params params ( ParamsConverter::convert ( value ) );
      lightModel = LightModel::RefPtr ( new LightModel ( lmn, params ) );
    }

    // If we set the light model then add it to the state.
    if ( true == lightModel.valid() )
    {
      shape.stateContainer()->add ( lightModel );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the material.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setMaterial ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;
    typedef SceneGraph::State::Attributes::Material Material;
    namespace Face = SceneGraph::State::Attributes::Face;
    typedef TreeNode::Children TreeChildren;
    typedef TreeChildren::const_iterator TreeItr;

    // Look for the material nodes.
    TreeNode::Children mc ( Tree::Algorithms::findAll 
      ( tn, std::string ( "material" ), false ) );

    // Loop through the children.
    for ( TreeItr i = mc.begin(); i != mc.end(); ++i )
    {
      const TreeNode::RefPtr &mn ( *i );
      if ( true == mn.valid() )
      {
        // Make default material properties.
        Material::RefPtr mat     ( new Material()   );
        Face::Enum     face      ( mat->face()      );
        Material::Vec4 ambient   ( mat->ambient()   );
        Material::Vec4 diffuse   ( mat->diffuse()   );
        Material::Vec4 specular  ( mat->specular()  );
        Material::Vec4 emission  ( mat->emission()  );
        float          shininess ( mat->shininess() );

        // Look for the properties.
        Helper::setProperty ( *mn, "face",      face      );
        Helper::setProperty ( *mn, "ambient",   ambient   );
        Helper::setProperty ( *mn, "diffuse",   diffuse   );
        Helper::setProperty ( *mn, "specular",  specular  );
        Helper::setProperty ( *mn, "emission",  emission  );
        Helper::setProperty ( *mn, "shininess", shininess );

        // Add a new material to the shape's state-container.
        mat = Material::RefPtr ( new Material 
          ( face, ambient, diffuse, specular, emission, shininess ) );
        shape.stateContainer()->add ( mat );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the shader.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setShader ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;

    // Set the shader if it's present.
    Shader::RefPtr shader;
    TreeNode::RefPtr sn ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "shader" ), false ) );
    if ( true == sn.valid() )
    {
      TreeNode::RefPtr program = Tree::Algorithms::findFirst 
        ( *sn, std::string ( "program" ), false );
      if ( true == program.valid() )
      {
        // Look for the shader with the program.
        shader = ShaderManager::instance().createShader ( program->value() );
      }
    }

    // Handle no shader.
    if ( false == shader.valid() )
      return;

    // Set the node's shader now.
    shape.stateContainer()->add ( shader );

    // Look for the parameters.
    TreeNode::RefPtr parameters ( Tree::Algorithms::findFirst 
      ( *sn, std::string ( "parameters" ), false ) );
    if ( false == parameters.valid() )
      return;

    // Loop through all the children that are parameters.
    TreeNode::Children params ( Tree::Algorithms::findAll 
      ( *parameters, std::string ( "parameter" ), false ) );
    typedef TreeNode::Children::iterator Itr;
    for ( Itr i = params.begin(); i != params.end(); ++i )
    {
      TreeNode::RefPtr param ( *i );
      if ( true == param.valid() )
      {
        // Get the name.
        TreeNode::RefPtr name ( Tree::Algorithms::findFirst 
          ( *param, std::string ( "name" ), false ) );

        // Get the type.
        TreeNode::RefPtr type ( Tree::Algorithms::findFirst 
          ( *param, std::string ( "type" ), false ) );

        // Get the value.
        TreeNode::RefPtr value ( Tree::Algorithms::findFirst 
          ( *param, std::string ( "value" ), false ) );

        // Need all three or we skip.
        if ( ( true == name.valid() ) &&
             ( true == type.valid() ) &&
             ( true == value.valid() ) )
        {
          // Set the parameter.
          if ( "float" == type->value() )
          {
            typedef Usul::Convert::Type < std::string, float > Converter;
            const float v ( Converter::convert ( value->value() ) );
            shader->paramSet ( name->value(), v );
          }
          else if ( "vec2" == type->value() )
          {
            typedef Usul::Math::Vec2f Vec;
            typedef Usul::Convert::Type < std::string, Vec > Converter;
            const Vec v ( Converter::convert ( value->value() ) );
            shader->paramSet ( name->value(), v );
          }
          else if ( "vec3" == type->value() )
          {
            typedef Usul::Math::Vec3f Vec;
            typedef Usul::Convert::Type < std::string, Vec > Converter;
            const Vec v ( Converter::convert ( value->value() ) );
            shader->paramSet ( name->value(), v );
          }
          else if ( "vec4" == type->value() )
          {
            typedef Usul::Math::Vec4f Vec;
            typedef Usul::Convert::Type < std::string, Vec > Converter;
            const Vec v ( Converter::convert ( value->value() ) );
            shader->paramSet ( name->value(), v );
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the shape.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setShape ( const Parser::TreeNode &tn, Shape &shape )
  {
    typedef Parser::TreeNode TreeNode;

    // Set the draw list index.
    {
      TreeNode::RefPtr dli ( Tree::Algorithms::findFirst 
        ( tn, std::string ( "draw_list_index" ), false ) );
      if ( true == dli.valid() )
      {
        typedef Usul::Convert::Type<std::string,int> Converter;
        shape.drawListIndexSet ( Converter::convert ( dli->value() ) );
      }
    }

    // Set the importance.
    {
      TreeNode::RefPtr in ( Tree::Algorithms::findFirst 
        ( tn, std::string ( "importance" ), false ) );
      if ( true == in.valid() )
      {
        typedef Usul::Convert::Type<std::string,int> Converter;
        shape.importanceSet ( Converter::convert ( in->value() ) );
      }
    }

    // Look for the state-container.
    StateContainer::RefPtr stateContainer;
    TreeNode::RefPtr ss ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "state" ), false ) );
    if ( false == ss.valid() )
      return;

    // Look for the shader.
    Helper::setShader ( *ss, shape );

    // Look for the fixed functionality.
    Helper::setMaterial    ( *ss, shape );
    Helper::setLight       ( *ss, shape );
    Helper::setPolygonMode ( *ss, shape );
    Helper::setShadeModel  ( *ss, shape );
    Helper::setLightModel  ( *ss, shape );
    Helper::setCullFace    ( *ss, shape );
    Helper::setBlending    ( *ss, shape );
    Helper::setEnable      ( *ss, shape, true );
    Helper::setEnable      ( *ss, shape, false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildGroup ( const TreeNode &treeNode )
{
  Group::RefPtr group ( new Group );
  Helper::buildGroup < Group > ( _handlers, treeNode, _instances, *group );
  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildTransform ( const TreeNode &tn )
{
  Transform::RefPtr transform ( new Transform );

  // Set the matrix if it's present.
  {
    const std::string name ( "matrix" );
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst ( tn, name, false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, Transform::Matrix > Converter;
      transform->matrixSet ( Converter::convert ( child->value() ) );
    }
  }

  // Set the rotation if it's present.
  {
    TreeNode::RefPtr rotation ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "rotation" ), false ) );
    if ( true == rotation.valid() )
    {
      TreeNode::RefPtr axis    ( Tree::Algorithms::findFirst ( *rotation, std::string ( "axis" ),    false ) );
      TreeNode::RefPtr degrees ( Tree::Algorithms::findFirst ( *rotation, std::string ( "degrees" ), false ) );
      if ( ( true == axis.valid() ) && ( true == degrees.valid() ) )
      {
        typedef Usul::Math::Vec3d Axis;
        typedef Usul::Convert::Type < std::string, Axis > AxisConverter;
        typedef Usul::Convert::Type < std::string, double > DegreeConverter;
        typedef Transform::Matrix Matrix;

        const Axis a ( AxisConverter::convert ( axis->value() ) );
        const double d ( DegreeConverter::convert ( degrees->value() ) );
        const double r ( Usul::Math::DEG_TO_RAD * d );
        const Matrix m ( Matrix::rotation ( r, a ) );
        transform->matrixSet ( m );
      }
    }
  }

  // Set the translation if it's present.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "translation" ), false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, Usul::Math::Vec3d > Converter;
      typedef Transform::Matrix Matrix;
      transform->matrixSet ( Matrix::translation ( Converter::convert ( child->value() ) ) );
    }
  }

  // Set the scale if it's present.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "scale" ), false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, Usul::Math::Vec3d > Converter;
      typedef Transform::Matrix Matrix;
      transform->matrixSet ( Matrix::scale ( Converter::convert ( child->value() ) ) );
    }
  }

  Helper::buildGroup < Transform > ( _handlers, tn, _instances, *transform );
  return transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  Geometry::Mode convertPrimitiveMode ( const std::string &s )
  {
    typedef Helper::GeometryModes ModeMap;
    typedef ModeMap::const_iterator Itr;
    ModeMap &mm ( Helper::geometryModes );
    Itr i ( mm.find ( s ) );
    return ( ( mm.end() == i ) ? Geometry::INVALID_MODE : i->second );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for adding a primitive.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void addPrimitive ( Tree::Node::RefPtr tnm, Tree::Node::RefPtr tni, Geometry &g )
  {
    typedef Geometry::Indices Indices;
    typedef Geometry::SharedPrimitives SharedPrimitives;
    typedef Geometry::Primitive Primitive;

    // Check input.
    if ( ( false == tnm.valid() ) || ( false == tni.valid() ) )
      return;

    // Read indices.
    Indices indices;
    {
      std::istringstream in ( tni->value() );
      Usul::IO::Text::Reader::read ( in, indices );
    }

    // Convert the string to a primitive mode.
    Geometry::Mode mode ( Helper::convertPrimitiveMode ( tnm->value() ) );

    // Check state.
    if ( ( true == indices.empty() ) || ( Geometry::INVALID_MODE == mode ) )
      return;

    // Add the primitive.
    g.primitiveAdd ( Primitive ( mode, indices ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildGeometry ( const TreeNode &tn )
{
  // Make the geometry and set node properties.
  Geometry::RefPtr geometry ( new Geometry );
  Helper::setShape ( tn, *geometry );

  // Is it named?
  Helper::registerInstance ( tn, *geometry, _instances );

  // Set the vertices if they are present.
  {
    const std::string name ( "vertices" );
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst ( tn, name, false ) );
    if ( true == child.valid() )
    {
      Geometry::Vertices vertices;
      std::istringstream in ( child->value() );
      Usul::IO::Text::Reader::read ( in, vertices );
      geometry->verticesSet ( new Geometry::SharedVertices ( vertices ) );
    }
  }

  // Set the normals if they are present.
  {
    const std::string name ( "normals" );
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst ( tn, name, false ) );
    if ( true == child.valid() )
    {
      Geometry::Normals normals;
      std::istringstream in ( child->value() );
      Usul::IO::Text::Reader::read ( in, normals );
      geometry->normalsSet ( new Geometry::SharedNormals ( normals ) );
    }
  }

  // Set the colors if they are present.
  {
    const std::string name ( "colors" );
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst ( tn, name, false ) );
    if ( true == child.valid() )
    {
      Geometry::Colors colors;
      std::istringstream in ( child->value() );
      Usul::IO::Text::Reader::read ( in, colors );
      geometry->colorsSet ( new Geometry::SharedColors ( colors ) );
    }
  }

  // Set the texture coordinates if they are present.
  {
    const std::string name ( "tex_coords" );
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst ( tn, name, false ) );
    if ( true == child.valid() )
    {
      Geometry::TexCoords texCoords;
      std::istringstream in ( child->value() );
      Usul::IO::Text::Reader::read ( in, texCoords );
      geometry->texCoordsSet ( new Geometry::SharedTexCoords ( texCoords ) );
    }
  }

  // Set the primitives if they are present.
  {
    TreeNode::RefPtr primitives ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "primitives" ), false ) );
    if ( true == primitives.valid() )
    {
      // Get all the children that are a primitive.
      TreeNode::Children c ( Tree::Algorithms::findAll 
        ( *primitives, std::string ( "primitive" ), false ) );

      // Loop through each primitive.
      for ( TreeNode::Children::iterator i = c.begin(); i != c.end(); ++i )
      {
        // Get the primitive.
        TreeNode::RefPtr primitive ( *i );
        if ( true == primitive.valid() )
        {
          // Get the mode and indices.
          TreeNode::RefPtr mode ( Tree::Algorithms::findFirst 
            ( *primitive, std::string ( "mode" ), false ) );
          TreeNode::RefPtr indices ( Tree::Algorithms::findFirst 
            ( *primitive, std::string ( "indices" ), false ) );

          // Add the primitive.
          Helper::addPrimitive ( mode, indices, *geometry );
        }
      }
    }
  }

  // Return the new geometry.
  return geometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildLine ( const TreeNode &treeNode )
{
  // Make the line and set any properties.
  Line::RefPtr line ( new Line );
  Helper::setShape ( treeNode, *line );

  // Is it named?
  Helper::registerInstance ( treeNode, *line, _instances );

  // Return new line.
  return line;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildInstance ( const TreeNode &treeNode )
{
  const std::string name ( treeNode.attribute ( "name" ) );
  Instances::iterator i ( _instances.find ( name ) );
  return ( ( _instances.end() != i ) ? i->second : SceneNode::RefPtr() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the sphere.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::_buildSphere ( const TreeNode &tn )
{
  // Make the builder.
  typedef SceneGraph::Builders::Sphere Builder;
  Builder builder;
  unsigned int numSubdivisions ( 1 );

  // Set the radius if the property is present.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst
      ( tn, std::string ( "radius" ), false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, Builder::Radius > Converter;
      builder.radius ( Converter::convert ( child->value() ) );
    }
  }

  // Set the center if the property is present.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst
      ( tn, std::string ( "center" ), false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, Builder::Center > Converter;
      builder.center ( Converter::convert ( child->value() ) );
    }
  }

  // Set the number of subdivisions if the property is present.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst
      ( tn, std::string ( "subdivisions" ), false ) );
    if ( true == child.valid() )
    {
      typedef Usul::Convert::Type < std::string, unsigned int > Converter;
      Converter::convert ( child->value(), numSubdivisions );
    }
  }

  // Make new sphere.
  typedef SceneGraph::Nodes::Shapes::Geometry Geometry;
  Geometry::RefPtr sphere ( builder.build ( numSubdivisions ) );

  // Set node properties.
  Helper::setShape ( tn, *sphere );

  // Is it named?
  Helper::registerInstance ( tn, *sphere, _instances );

  // Set the sorting callback.
  {
    TreeNode::RefPtr child ( Tree::Algorithms::findFirst 
      ( tn, std::string ( "sort_triangles" ), false ) );
    if ( true == child.valid() )
    {
      if ( "back_to_front" == boost::algorithm::to_lower_copy ( child->value() ) )
      {
        sphere->sortPrimitivesCallbackSet ( sphere->sortPrimitivesBackToFront() );
      }
    }
  }

  // Return new sphere.
  return sphere;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::read 
  ( const std::string &name, IUnknown::RefPtr caller )
{
  _threadCheck.throwIfDifferentThread();

  if ( false == boost::filesystem::exists ( name ) )
  {
    throw std::invalid_argument ( Usul::Strings::format 
      ( "Error 2816987519: Given file '", name, "' does not exist" ) );
  }

  const Usul::Types::UInt64 size ( boost::filesystem::file_size ( name ) );

  std::ifstream in ( name.c_str() );
  if ( false == in.is_open() )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 4068965459: Unable to open file '", name, "' for reading" ) );
  }

  return this->read ( in, size, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the top-level node.
//
///////////////////////////////////////////////////////////////////////////////

Parser::SceneNode::RefPtr Parser::read 
  ( std::istream &in, Usul::Types::UInt64 streamSize, IUnknown::RefPtr caller )
{
  _threadCheck.throwIfDifferentThread();

  // Clear the instances.
  _instances.clear();
  Usul::Scope::Caller::RefPtr clearInstances ( Usul::Scope::makeCaller 
    ( boost::bind ( &Instances::clear, boost::ref ( _instances ) ) ) );

  // Constant used below.
  const std::string rootNodeName ( "haf_scene_graph" );

  // Find an XML reader.
  typedef Tree::Interfaces::IReadXML IReadXML;
  Usul::Plugins::Manager &pm ( Usul::Plugins::Manager::instance() );
  IReadXML::QueryPtr reader ( pm.findFirst ( IReadXML::IID ) );
  if ( false == reader.valid() )
  {
    throw std::runtime_error 
      ( "Error 1410228054: Unable to find XML reader" );
  }

  // Read the stream.
  TreeNode::RefPtr root ( reader->readXmlFile ( in, streamSize, caller ) );
  if ( false == root.valid() )
  {
    throw std::runtime_error 
      ( "Error 1887988117: Invalid node returned from XML reader" );
  }

  // Should recognize the root node's name.
  if ( rootNodeName != root->name() )
  {
    throw std::runtime_error ( Usul::Strings::format (
      "Error 2473554750: Expected root node to be named '", 
      rootNodeName, "', found '", root->name(), "'" ) );
  }

  // Should have one child.
  if ( 1 != root->numChildren() )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 2017178653: Expected 1 child, found ", root->numChildren() ) );
  }

  // We want the first child.
  root = root->children().front();
  if ( false == root.valid() )
  {
    throw std::runtime_error ( "Error 4902195800: Null child of root node" );
  }

  // Traverse the tree-node.
  SceneNode::RefPtr node ( Helper::traverse ( _handlers, *root ) );

  // Return the new node.
  return node;
}
