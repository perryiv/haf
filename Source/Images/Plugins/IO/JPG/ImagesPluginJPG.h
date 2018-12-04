
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

#ifndef _IMAGES_IO_JPG_COMPONENT_CLASS_H_
#define _IMAGES_IO_JPG_COMPONENT_CLASS_H_

#include "Images/Plugins/IO/JPG/CompileGuard.h"
#include "Images/Interfaces/IImageRead.h"
#include "Images/Interfaces/IImageWrite.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "boost/noncopyable.hpp"


namespace Images {
namespace Plugins {
namespace IO {


class ImagesPluginJPG : 
  public Usul::Base::Referenced,
  public boost::noncopyable,
  public Usul::Interfaces::IPlugin,
  public Images::Interfaces::IImageRead,
  public Images::Interfaces::IImageWrite
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Images::Core::BaseImage::RefPtr ImagePtr;

  // Smart-pointer definitions.
  USUL_REFERENCED_CLASS ( ImagesPluginJPG );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  ImagesPluginJPG();

  // Usul::Interfaces::IPlugin
  virtual std::string             pluginDescription() const;
  virtual std::string             pluginName() const { return "Images IO JPG"; }
  virtual void                    pluginInitialize(){}
  virtual void                    pluginFinalize(){}

  // Images::Interfaces::IImageRead
  virtual bool                    canReadImageFile ( const std::string &file );
  virtual ImagePtr                readImageFile ( 
                                    const std::string &file, 
                                    IUnknown::RefPtr caller );

  // Images::Interfaces::IImageWrite
  virtual bool                    canWriteImageFile ( 
                                    ImagePtr image, 
                                    const std::string &file );
  virtual void                    writeImageFile ( 
                                    ImagePtr image, 
                                    const std::string &file, 
                                    IUnknown::RefPtr caller );

protected: 

  // Use reference counting.
  virtual ~ImagesPluginJPG();
};


} // namespace IO
} // namespace Plugins
} // namespace Images


#endif // _IMAGES_IO_JPG_COMPONENT_CLASS_H_
