
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes that represent attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_ATTRIBUTES_CLASSES_H_
#define _SCENE_GRAPH_ATTRIBUTES_CLASSES_H_

#include "SceneGraph/State/Attributes/BaseAttribute.h"


namespace SceneGraph {
namespace State {
namespace Attributes {


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for color.
//
///////////////////////////////////////////////////////////////////////////////

class Color : public ExclusiveAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( Color, ExclusiveAttribute );
  typedef Usul::Math::Vec4d Vec4;
  Color ( const Vec4 &c ) : BaseClass(), _c ( c ){}
  Color ( double r, double g, double b, double a ) : _c ( r, g, b, a ){}
  const Vec4 &color() const { return _c; }
private:
  const Vec4 _c;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for light.
//
///////////////////////////////////////////////////////////////////////////////

class Light : public NonExclusiveAttribute
{
public:
  enum Parameter
  {
    AMBIENT = 0, DIFFUSE, SPECULAR, POSITION,
    SPOT_DIRECTION, SPOT_EXPONENT, SPOT_CUTOFF,
    CONSTANT_ATTENUATION, LINEAR_ATTENUATION, QUADRATIC_ATTENUATION
  };
  typedef Usul::Math::Vec4f Value;
  typedef Usul::Math::Vec3f Vec3f;
  SCENE_GRAPH_ATTRIBUTE ( Light, NonExclusiveAttribute );
  Light ( unsigned int source, Parameter p, bool v ) : 
    BaseClass(), _s ( source ), _p ( p ), _v ( v, v, v, v ){}
  Light ( unsigned int source, Parameter p, float v ) : 
    BaseClass(), _s ( source ), _p ( p ), _v ( v, v, v, v ){}
  Light ( unsigned int source, Parameter p, const Vec3f &v ) : 
    BaseClass(), _s ( source ), _p ( p ), _v ( v[0], v[1], v[2], 0 ){}
  Light ( unsigned int source, Parameter p, const Value &v ) : 
    BaseClass(), _s ( source ), _p ( p ), _v ( v ){}
  unsigned int source() const { return _s; }
  Parameter parameter() const { return _p; }
  const Value &value() const { return _v; }
  virtual unsigned int key() const
  {
    return ( ( 1000 * this->source() ) + this->parameter() );
  }
private:
  unsigned int _s;
  const Parameter _p;
  const Value _v;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for light model.
//
///////////////////////////////////////////////////////////////////////////////

class LightModel : public NonExclusiveAttribute
{
public:
  enum Name { AMBIENT = 0, COLOR_CONTROL, LOCAL_VIEWER, TWO_SIDE };
  typedef Usul::Math::Vec4f Params;
  SCENE_GRAPH_ATTRIBUTE ( LightModel, NonExclusiveAttribute );
  LightModel ( Name n, bool p ) : BaseClass(), _n ( n ), _p ( p, p, p, p ){}
  LightModel ( Name n, float p ) : BaseClass(), _n ( n ), _p ( p, p, p, p ){}
  LightModel ( Name n, const Params &p ) : BaseClass(), _n ( n ), _p ( p ){}
  Name name() const { return _n; }
  const Params &params() const { return _p; }
  virtual unsigned int key() const { return this->name(); }
private:
  const Name _n;
  const Params _p;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for material.
//
///////////////////////////////////////////////////////////////////////////////

class Material : public NonExclusiveAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( Material, NonExclusiveAttribute );
  typedef Usul::Math::Vec4f Vec4;
  Material ( Face::Enum f = Face::FRONT,
             const Vec4 &a = Vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ),
             const Vec4 &d = Vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ),
             const Vec4 &s = Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ),
             const Vec4 &e = Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ),
             float sh = 0 ) : BaseClass(), 
             _f ( f ), _a ( a ), _d ( d ), _s ( s ), _e ( e ), _sh ( sh ){}
  Face::Enum face() const { return _f; }
  const Vec4 &ambient() const { return _a; }
  const Vec4 &diffuse() const { return _d; }
  const Vec4 &specular() const { return _s; }
  const Vec4 &emission() const { return _e; }
  float shininess() const { return _sh; }
  virtual unsigned int key() const { return this->face(); }
private:
  const Face::Enum _f;
  const Vec4 _a;
  const Vec4 _d;
  const Vec4 _s;
  const Vec4 _e;
  const float _sh;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for culling faces.
//
///////////////////////////////////////////////////////////////////////////////

class CullFace : public ExclusiveAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( CullFace, ExclusiveAttribute );
  CullFace ( Face::Enum f ) : BaseClass(), _f ( f ){}
  Face::Enum face() const { return _f; }
private:
  const Face::Enum _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

class PolygonMode : public ExclusiveAttribute
{
public:
  enum Mode { POINTS = 0, LINES, FILLED };
  SCENE_GRAPH_ATTRIBUTE ( PolygonMode, ExclusiveAttribute );
  PolygonMode ( Face::Enum f, Mode m ) : BaseClass(), _f ( f ), _m ( m ){}
  Face::Enum face() const { return _f; }
  Mode mode() const { return _m; }
private:
  const Face::Enum _f;
  const Mode _m;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for shade model.
//
///////////////////////////////////////////////////////////////////////////////

class ShadeModel : public ExclusiveAttribute
{
public:
  enum Model { FLAT = 0, SMOOTH };
  SCENE_GRAPH_ATTRIBUTE ( ShadeModel, ExclusiveAttribute );
  ShadeModel ( Model m ) : BaseClass(), _m ( m ){}
  Model model() const { return _m; }
private:
  const Model _m;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for line width.
//
///////////////////////////////////////////////////////////////////////////////

class LineWidth : public ExclusiveAttribute
{
public:
  SCENE_GRAPH_ATTRIBUTE ( LineWidth, ExclusiveAttribute );
  LineWidth ( float width ) : BaseClass(), _w ( width ){}
  float width() const { return _w; }
private:
  const float _w;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for enabling.
//
///////////////////////////////////////////////////////////////////////////////

class Enable : public NonExclusiveAttribute
{
public:
  enum Mode
  {
    // Server modes.
    ALPHA_TEST = 0, AUTO_NORMAL, BLEND, CLIP_PLANE0, CLIP_PLANE1, 
    CLIP_PLANE2, CLIP_PLANE3, CLIP_PLANE4, CLIP_PLANE5, 
    COLOR_LOGIC_OP, COLOR_MATERIAL, COLOR_SUM, COLOR_TABLE, 
    CONVOLUTION_1D, CONVOLUTION_2D, CULL_FACE, DEPTH_TEST, 
    DITHER, FOG, HISTOGRAM, INDEX_LOGIC_OP, LIGHT0, LIGHT1, LIGHT2, 
    LIGHT3, LIGHT4, LIGHT5, LIGHT6, LIGHT7, LIGHTING, LINE_SMOOTH, 
    LINE_STIPPLE, MAP1_COLOR_4, MAP1_INDEX, MAP1_NORMAL, 
    MAP1_TEXTURE_COORD_1, MAP1_TEXTURE_COORD_2, MAP1_TEXTURE_COORD_3, 
    MAP1_TEXTURE_COORD_4, MAP1_VERTEX_3, MAP1_VERTEX_4, MAP2_COLOR_4, 
    MAP2_INDEX, MAP2_NORMAL, MAP2_TEXTURE_COORD_1, MAP2_TEXTURE_COORD_2, 
    MAP2_TEXTURE_COORD_3, MAP2_TEXTURE_COORD_4, MAP2_VERTEX_3, MAP2_VERTEX_4, 
    MINMAX, MULTISAMPLE, NORMALIZE, POINT_SMOOTH, POINT_SPRITE, 
    POLYGON_OFFSET_FILL, POLYGON_OFFSET_LINE, POLYGON_OFFSET_POINT, 
    POLYGON_SMOOTH, POLYGON_STIPPLE, POST_COLOR_MATRIX_COLOR_TABLE, 
    POST_CONVOLUTION_COLOR_TABLE, RESCALE_NORMAL, SAMPLE_ALPHA_TO_COVERAGE, 
    SAMPLE_ALPHA_TO_ONE, SAMPLE_COVERAGE, SEPARABLE_2D, SCISSOR_TEST, 
    STENCIL_TEST, TEXTURE_1D, TEXTURE_2D, TEXTURE_3D, TEXTURE_CUBE_MAP, 
    TEXTURE_GEN_Q, TEXTURE_GEN_R, TEXTURE_GEN_S, TEXTURE_GEN_T, 
    VERTEX_PROGRAM_POINT_SIZE, VERTEX_PROGRAM_TWO_SIDE,

    // Client modes.
    COLOR_ARRAY, EDGE_FLAG_ARRAY, INDEX_ARRAY, 
    NORMAL_ARRAY, TEXTURE_COORD_ARRAY, VERTEX_ARRAY
  };
  SCENE_GRAPH_ATTRIBUTE ( Enable, NonExclusiveAttribute );
  Enable ( Mode m, bool state ) : BaseClass(), _m ( m ), _s ( state ){}
  Mode mode() const { return _m; }
  bool state() const { return _s; }
  virtual unsigned int key() const { return this->mode(); }
private:
  const Mode _m;
  const bool _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for hints.
//
///////////////////////////////////////////////////////////////////////////////

class Hint : public NonExclusiveAttribute
{
public:
  enum Target
  {
    FOG_HINT = 0, GENERATE_MIPMAP_HINT, LINE_SMOOTH_HINT, 
    PERSPECTIVE_CORRECTION_HINT, POINT_SMOOTH_HINT, POLYGON_SMOOTH_HINT, 
    TEXTURE_COMPRESSION_HINT, FRAGMENT_SHADER_DERIVATIVE_HINT
  };
  enum Mode
  {
    FASTEST = 0, NICEST, DONT_CARE
  };
  SCENE_GRAPH_ATTRIBUTE ( Hint, NonExclusiveAttribute );
  Hint ( Target t, Mode m ) : BaseClass(), _t ( t ), _m ( m ){}
  Target target() const { return _t; }
  Mode mode() const { return _m; }
  virtual unsigned int key() const
  {
    return ( ( 1000 * this->target() ) + this->mode() );
  }
private:
  const Target _t;
  const Mode _m;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for blending function.
//
///////////////////////////////////////////////////////////////////////////////

class Blending : public ExclusiveAttribute
{
public:
  enum Factor
  {
    ZERO = 0, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, 
    ONE_MINUS_DST_COLOR, SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, 
    ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR, 
    CONSTANT_ALPHA, ONE_MINUS_CONSTANT_ALPHA, SRC_ALPHA_SATURATE
  };
  SCENE_GRAPH_ATTRIBUTE ( Blending, ExclusiveAttribute );
  Blending ( Factor s, Factor d ) : BaseClass(), _s ( s ), _d ( d ){}
  Factor source() const { return _s; }
  Factor destination() const { return _d; }
private:
  const Factor _s;
  const Factor _d;
};


} // namespace Attributes
} // namespace State
} // namespace SceneGraph


#endif // _SCENE_GRAPH_ATTRIBUTES_CLASSES_H_
