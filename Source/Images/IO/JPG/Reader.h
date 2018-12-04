
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For reading a JPG file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IO_JPG_READER_CLASS_H_
#define _IMAGES_IO_JPG_READER_CLASS_H_

#include "Images/IO/JPG/Base.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"

#include "boost/shared_ptr.hpp"

#ifdef _MSC_VER
# pragma warning ( disable : 4005 )
#endif

extern "C"
{
  #include "jpeglib.h"
}

#include <cstdio>
#include <stdexcept>


namespace Images {
namespace IO {
namespace JPG {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading a JPG file.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ImageType_,
  class ProgressFunctor_ = DefaultProgressFunctor
>
class Reader : public Base
{
public:

  typedef Base BaseClass;
  typedef ImageType_ ImageType;
  typedef ProgressFunctor_ Progress;
  typedef typename ImageType::RefPtr ImagePtr;
  typedef typename ImageType::Data Data;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reads the jpeg file and return the new image.
  //  See: http://svn.ghostscript.com/ghostscript/tags/jpeg-6b/example.c
  //
  /////////////////////////////////////////////////////////////////////////////

  static ImagePtr readImageFile ( 
    const std::string &name, 
    Progress progress = Progress() )
  {
    // Need to declare in this scope.
    ImageType::RefPtr image;

    USUL_TRY_BLOCK
    {
      // Set up reading with the jpeg library.
      jpeg_decompress_struct cinfo;
      jpeg_error_mgr jerr;
      cinfo.err = jpeg_std_error ( &jerr );
      cinfo.err->error_exit = &BaseClass::_jpegError;
      ::jpeg_create_decompress ( &cinfo );

      // Need local scope.
      {
        // Open file and set up decompression.
        boost::shared_ptr < FILE > file 
          ( ::fopen ( name.c_str(), "rb" ), ::fclose );
        ::jpeg_stdio_src ( &cinfo, file.get() );
        ::jpeg_read_header ( &cinfo, TRUE );
        ::jpeg_start_decompress ( &cinfo );

        // Needed below.
        const unsigned int width ( cinfo.output_width );
        const unsigned int height ( cinfo.output_height );
        const unsigned int channels ( cinfo.out_color_components );
        JSAMPROW ptr[1] = { 0x0 };

        // Make our image and get shortcut to the data.
        image = ImagePtr ( new ImageType ( width, height, channels ) );
        Data &data ( image->data() );

        // Read all the scan lines.
        for ( unsigned int i = 0; i < height; ++i )
        {
          const unsigned int offset ( i * width * channels );
          ptr[0] = ( &data[0] + offset );
          ::jpeg_read_scanlines ( &cinfo, ptr, 1 );
          progress ( i + 1, height );
        }
      }

      // Clean up.
      ::jpeg_finish_decompress ( &cinfo );
      ::jpeg_destroy_decompress ( &cinfo );
    }
    catch ( const Usul::Exceptions::Error &e )
    {
      throw Usul::Exceptions::Error ( "8935358060", Usul::Strings::format ( 
        "Failed to read file: ", name ), e );
    }
    catch ( const std::exception &e )
    {
      throw Usul::Exceptions::Error ( "2620663637", Usul::Strings::format ( 
        "Failed to read file: ", name ), e );
    }
    catch ( ... )
    {
      throw Usul::Exceptions::Error ( "4082697744", Usul::Strings::format (
        "Failed to open file: ", name ) );
    }

    // Return the image.
    return ImagePtr ( image );
  }
};


} // namespace JPG
} // namespace IO
} // namespace Images


#endif // _IMAGES_IO_JPG_READER_CLASS_H_
