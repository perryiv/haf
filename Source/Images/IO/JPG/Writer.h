
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For writing a JPG file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IO_JPG_WRITER_CLASS_H_
#define _IMAGES_IO_JPG_WRITER_CLASS_H_

#include "Images/IO/JPG/Base.h"
#include "Images/IO/JPG/MemoryOutput.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Functions/NoThrow.h"
#include "Usul/System/LastError.h"

#include "boost/shared_ptr.hpp"

#ifdef _MSC_VER
# pragma warning ( disable : 4005 )
#endif

extern "C"
{
  #include "jpeglib.h"
}

#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <setjmp.h>


namespace Images {
namespace IO {
namespace JPG {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for writing a JPG file.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ImageType_,
  class ProgressFunctor_ = DefaultProgressFunctor
>
class Writer : public Base
{
public:

  typedef Base BaseClass;
  typedef ImageType_ ImageType;
  typedef ProgressFunctor_ Progress;
  typedef typename ImageType::RefPtr ImagePtr;
  typedef typename ImageType::Data Data;
  typedef typename ImageType::ElementType ElementType;
  typedef MemoryOutput::Output MemoryBuffer;
  typedef Usul::Exceptions::Error Exception;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Write the image to a buffer.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void writeImageFile ( 
    const ImagePtr image,
    MemoryBuffer &buffer, 
    unsigned int quality = 100,
    Progress progress = Progress() )
  {
    // Initialize the system's last error.
    Usul::System::LastError::init();

    // Handle bad input.
    if ( false == image.valid() )
    {
      throw Exception ( "4144231571", "Given image is null" );
    }

    // Has to be a 2D image.
    if ( 2 != image->dimensions() )
    {
      throw Exception ( "2442344154", "Image is not 2-dimensional" );
    }

    // Has to be 8-bit elements in the data vector.
    if ( 1 != image->numBytesPerElement() )
    {
      throw Exception ( "2282707085","Image does not have 8-bit elements" );
    }

    // Has to be a reasonable size.
    const unsigned int width ( image->width() );
    const unsigned int height ( image->height() );
    if ( ( 0 == width ) || ( 0 == height ) )
    {
      throw Exception ( "3296188893", Usul::Strings::format (
        "Given image size is ", width, " x ", height ) );
    }

    // Check number of channels.
    J_COLOR_SPACE colorSpace ( JCS_UNKNOWN );
    switch ( image->numChannels() )
    {
      case 1:
      {
        colorSpace = JCS_GRAYSCALE;
        break;
      }
      case 3:
      {
        colorSpace = JCS_RGB;
        break;
      }
      default:
      {
        throw Exception ( "3507516290", Usul::Strings::format (
          "Number of channels '", image->numChannels(), "' is not supported" ) );
      }
    }

    // Check the quality.
    if ( quality > 100 )
    {
      throw Exception ( "3769879198", "Quality is not in the range [0,100]" );
    }

    // Check the data.
    if ( 0x0 == image->bytes() )
    {
      throw Exception ( "4127094856", "Image has null data" );
    }

    // Reserve space in the buffer to avoid multiple re-allocations.
    buffer.reserve ( width * height + 512 );

    // Set up writing with the jpeg library.
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    ::memset ( &cinfo, 0, sizeof ( jpeg_compress_struct ) );
    ::memset ( &jerr, 0, sizeof ( jpeg_error_mgr ) );
    cinfo.err = jpeg_std_error ( &jerr );
    cinfo.err->error_exit = &Writer::_jpegError;

    // Save stack location.
    jmp_buf env;
    if ( ::setjmp ( env ) ) // Search code for "2159138372"
    {
      const std::string message ( BaseClass::getErrorMessage ( cinfo ) );
      ::jpeg_destroy_compress ( &cinfo );
      throw Exception ( "6256921700", message );
    }

    // Needed so we can jump out of error handlers.
    cinfo.client_data = static_cast < void * > ( &env );

    // Create the compressor.
    ::jpeg_create_compress ( &cinfo );

    // Set the output of compression to tbe buffer.
    MemoryOutput::set ( &cinfo, &buffer );

    // Set the fields for the header.
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = image->numChannels();
    cinfo.in_color_space = colorSpace;

    // Set default compression parameters.
    jpeg_set_defaults ( &cinfo );

    // Set the quality.
    jpeg_set_quality ( &cinfo, static_cast < int > ( quality ), TRUE );

    // Start the compressor.
    ::jpeg_start_compress ( &cinfo, TRUE );

    // Needed below.
    JSAMPROW ptr[1] = { 0x0 };
    const unsigned int stride ( cinfo.image_width * cinfo.input_components );
    const Data &data ( image->data() );

    // Loop through the scan-lines.
    while ( cinfo.next_scanline < cinfo.image_height )
    {
      const unsigned int offset ( cinfo.next_scanline * stride );
      ptr[0] = const_cast < ElementType * > ( &data[0] + offset );
      ::jpeg_write_scanlines ( &cinfo, ptr, 1 );
      progress ( cinfo.next_scanline, cinfo.image_height );
    }

    // Finish the job.
    ::jpeg_finish_compress ( &cinfo );

    // Clean up.
    ::jpeg_destroy_compress ( &cinfo );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Write the image to a jpeg file.
  //  See: http://svn.ghostscript.com/ghostscript/tags/jpeg-6b/example.c
  //  And: http://www.codeguru.com/Cpp/G-M/bitmap/otherformats/article.php/c1731
  //
  /////////////////////////////////////////////////////////////////////////////

  static void writeImageFile ( 
    const ImagePtr image,
    const std::string &name, 
    unsigned int quality = 100,
    Progress progress = Progress() )
  {
    // Initialize the system's last error.
    Usul::System::LastError::init();

    if ( true == name.empty() )
    {
      throw Exception ( "2004824220", "Given file name is empty" );
    }

    // Make a buffer for the image.
    MemoryBuffer buffer;

    // Call the other one.
    writeImageFile ( image, buffer, quality, progress );

    // Write the buffer to file.
    std::ofstream file ( name.c_str(), std::ofstream::out | std::ofstream::binary );
    if ( false == file.is_open() )
    {
      throw Exception ( "2316235891", Usul::Strings::format ( 
        "Failed to open file '", name, "' for writing, ",
        Usul::System::LastError::message() ) );
    }

    // Write the buffer to file.
    file.write ( &buffer[0], buffer.size() );
  }
};


} // namespace JPG
} // namespace IO
} // namespace Images


#endif // _IMAGES_IO_JPG_WRITER_CLASS_H_
