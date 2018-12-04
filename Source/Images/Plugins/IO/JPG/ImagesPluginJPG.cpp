
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

#ifdef _MSC_VER
# pragma warning ( disable : 4204 ) // missing debugging information
#endif

#include "Images/Plugins/IO/JPG/ImagesPluginJPG.h"
#include "Images/Core/Image2.h"
#include "Images/IO/JPG/Reader.h"
#include "Images/IO/JPG/Writer.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/Plugins/Helper.h"
#include "Usul/Strings/Format.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/filesystem.hpp"

using namespace Images::Plugins::IO;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ImagesPluginJPG, BaseClass );
USUL_PLUGIN_INITIALIZE ( ImagesPluginJPG );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ImagesPluginJPG::ImagesPluginJPG() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ImagesPluginJPG::~ImagesPluginJPG()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ImagesPluginJPG::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Images::Interfaces::IImageRead::IID:
    return static_cast < Images::Interfaces::IImageRead * > ( this );
  case Images::Interfaces::IImageWrite::IID:
    return static_cast < Images::Interfaces::IImageWrite * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return description string.
//
/////////////////////////////////////////////////////////////////////////////

std::string ImagesPluginJPG::pluginDescription() const
{
  return "Image library plugin for reading and writing JPG files";
}


/////////////////////////////////////////////////////////////////////////////
//
//  Is the extension correct?
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool isCorrectExtension ( const std::string &name )
  {
    const std::string ext ( boost::algorithm::to_lower_copy 
      ( boost::filesystem::extension ( name ) ) );
    return ( ( ".jpg" == ext ) || ( ".jpeg" == ext ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Can we read the file?
//
/////////////////////////////////////////////////////////////////////////////

bool ImagesPluginJPG::canReadImageFile ( const std::string &name )
{
  return Helper::isCorrectExtension ( name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Can we write the file?
//
/////////////////////////////////////////////////////////////////////////////

bool ImagesPluginJPG::canWriteImageFile 
  ( ImagePtr image, const std::string &name )
{
  // Has to be a 2D image.
  typedef Images::Core::Image2 < unsigned char > Image2;
  Image2::RefPtr image2 ( dynamic_cast < Image2 * > ( image.get() ) );
  if ( true == image2.valid() )
  {
    return Helper::isCorrectExtension ( name );
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the new image.
//
/////////////////////////////////////////////////////////////////////////////

ImagesPluginJPG::ImagePtr ImagesPluginJPG::readImageFile (
  const std::string &name, 
  IUnknown::RefPtr caller )
{
  typedef Images::Core::Image2 < unsigned char > Image2;
  typedef Images::IO::JPG::Reader<Image2> Reader;
  return Reader::readImageFile ( name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the image to file.
//
/////////////////////////////////////////////////////////////////////////////

void ImagesPluginJPG::writeImageFile ( 
  ImagePtr image, 
  const std::string &name, 
  IUnknown::RefPtr caller )
{
  // Has to be a 2D image.
  typedef Images::Core::Image2 < unsigned char > Image2;
  Image2::RefPtr image2 ( dynamic_cast < Image2 * > ( image.get() ) );
  if ( false == image2.valid() )
  {
    throw Usul::Exceptions::Error ( "4275185415", Usul::Strings::format ( 
      "Failed to write file '", name, 
      "' because given image is not 2D unsigned char" ) );
  }

  // Write the image.
  typedef Images::IO::JPG::Writer<Image2> Writer;
  Writer::writeImageFile ( image2, name );
}
