
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Attribute classes for textures.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_TEXTURE_ATTRIBUTE_CLASSES_H_
#define _SCENE_GRAPH_TEXTURE_ATTRIBUTE_CLASSES_H_

#include "SceneGraph/State/Attributes/BaseAttribute.h"

#include "Images/Core/Image2.h"

#include "Usul/Atomic/Container.h"

#include <map>


namespace SceneGraph {
namespace State {
namespace Attributes {


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for a texture.
//
///////////////////////////////////////////////////////////////////////////////

class Texture : public ExclusiveAttribute
{
public:

  SCENE_GRAPH_ATTRIBUTE ( Texture, ExclusiveAttribute );
  typedef Usul::Base::Referenced RendererData;
  typedef std::map < unsigned long, RendererData::RefPtr > RendererDataMap;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;

  RendererData::RefPtr rendererDataGet ( unsigned long key ) const
  {
    Guard guard ( _rdm.mutex() );
    const RendererDataMap &rdm ( _rdm.getReference() );
    RendererDataMap::const_iterator i ( rdm.find ( key ) );
    return ( ( rdm.end() == i ) ? RendererData::RefPtr() : i->second );
  }

  void rendererDataSet ( unsigned long key, RendererData::RefPtr data )
  {
    Guard guard ( _rdm.mutex() );
    RendererDataMap &rdm ( _rdm.getReference() );
    rdm[key] = data;
  }

  void rendererDataRemove ( unsigned long key )
  {
    Guard guard ( _rdm.mutex() );
    RendererDataMap &rdm ( _rdm.getReference() );
    rdm.erase ( key );
  }

protected:

  Texture() : BaseClass(), _rdm()
  {
  }

private:

  Usul::Atomic::Container < RendererDataMap > _rdm;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Attribute class for a 2D texture.
//
///////////////////////////////////////////////////////////////////////////////

class Texture2 : public Texture
{
public:

  typedef Images::Core::Image2 < unsigned char > Image;
  typedef Usul::Atomic::Object < Image::RefPtr > AtomicImagePtr;

  SCENE_GRAPH_ATTRIBUTE ( Texture2, Texture );

  Texture2 ( Image::RefPtr image = Image::RefPtr() ) : 
    BaseClass(),
    _image ( image )
  {
  }

  void imageSet ( Image::RefPtr image )
  {
    _image = image;
  }

  AtomicImagePtr &imageGet()
  {
    return _image;
  }

protected:

  AtomicImagePtr _image;
};


} // namespace Attributes
} // namespace State
} // namespace SceneGraph


#endif // _SCENE_GRAPH_TEXTURE_ATTRIBUTE_CLASSES_H_
