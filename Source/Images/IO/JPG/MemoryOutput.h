
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Destination manager for memory output.
//  See: http://code.google.com/p/page-speed/source/browse/lib/trunk/src/pagespeed/image_compression/jpeg_optimizer.cc
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IO_JPG_WRITER_MEMORY_OUTPUT_CLASS_H_
#define _IMAGES_IO_JPG_WRITER_MEMORY_OUTPUT_CLASS_H_

#include "Usul/Exceptions/Exceptions.h"

#ifdef _MSC_VER
# pragma warning ( disable : 4005 )
#endif

extern "C"
{
  #include "jpeglib.h"
}

#include <stdexcept>
#include <vector>


namespace Images {
namespace IO {
namespace JPG {


struct MemoryOutput : public jpeg_destination_mgr
{
  typedef jpeg_destination_mgr BaseClass;
  typedef std::vector < char > Output;
  typedef Usul::Exceptions::Error Exception;
  enum { BUFFER_SIZE = 4096 };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Initialize the compressor's buffer.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void init ( j_compress_ptr cinfo )
  {
    MemoryOutput *out ( reinterpret_cast < MemoryOutput * > ( cinfo->dest ) );
    if ( 0x0 == out )
      return;

    out->next_output_byte = out->_buffer;
    out->free_in_buffer = BUFFER_SIZE;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Flush the compressor's buffer.
  //
  /////////////////////////////////////////////////////////////////////////////

  static boolean flush ( j_compress_ptr cinfo )
  {
    MemoryOutput *out ( reinterpret_cast < MemoryOutput * > ( cinfo->dest ) );
    if ( 0x0 == out )
      return FALSE;

    if ( 0x0 != out->_output )
    {
      out->_output->insert 
        ( out->_output->end(), out->_buffer, out->_buffer + BUFFER_SIZE );
    }

    out->next_output_byte = out->_buffer;
    out->free_in_buffer = BUFFER_SIZE;

    return TRUE;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Finish writing the compressor's buffer.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void finish ( j_compress_ptr cinfo )
  {
    MemoryOutput *out ( reinterpret_cast < MemoryOutput * > ( cinfo->dest ) );
    if ( 0x0 == out )
      return;

    const unsigned int count ( BUFFER_SIZE - out->free_in_buffer );
    if ( ( count > 0 ) && ( 0x0 != out->_output ) )
    {
      out->_output->insert 
        ( out->_output->end(), out->_buffer, out->_buffer + count );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the compressor's buffer.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void set ( j_compress_ptr cinfo, Output *output )
  {
    if ( 0x0 == cinfo->dest )
    {
      cinfo->dest = reinterpret_cast < jpeg_destination_mgr * > (
        (*cinfo->mem->alloc_small) ( 
          reinterpret_cast < j_common_ptr > ( cinfo ), 
          JPOOL_PERMANENT, 
          sizeof ( MemoryOutput ) ) );
    }

    MemoryOutput *out ( reinterpret_cast < MemoryOutput * > ( cinfo->dest ) );
    if ( 0x0 == out )
    {
      throw Exception ( "4069623804", "Failed to create output manager" );
    }
  
    out->_output = output;
    out->init_destination = &MemoryOutput::init;
    out->empty_output_buffer = &MemoryOutput::flush;
    out->term_destination = &MemoryOutput::finish;
  }

private:

  unsigned char _buffer[BUFFER_SIZE];
  Output *_output;
};


} // namespace JPG
} // namespace IO
} // namespace Images


#endif // _IMAGES_IO_JPG_WRITER_MEMORY_OUTPUT_CLASS_H_
