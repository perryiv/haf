
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_LIBRARY_BASE_IMAGE_CLASS_H_
#define _IMAGES_LIBRARY_BASE_IMAGE_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"

#include "boost/noncopyable.hpp"


namespace Images {
namespace Core {


class BaseImage : 
  public Usul::Base::Object,
  public boost::noncopyable
{
public:

  typedef Usul::Base::Object BaseClass;

  USUL_REFERENCED_CLASS ( BaseImage );

  // Return the raw bytes.
  virtual const unsigned char *bytes() const
  {
    return 0x0;
  }

  // Return the raw bytes at the given offset.
  const unsigned char *bytes ( unsigned int offset ) const
  {
    // Handle out-of-range offset.
    if ( offset >= this->numBytes() )
    {
      throw Usul::Exceptions::Error ( "2571846315", Usul::Strings::format (
        "Requested offset '", offset, "' is out of range" ) );
    }

    // Return the address.
    return ( this->bytes() + offset );
  }

  // Return the number of channels.
  virtual unsigned int numChannels() const
  {
    return 0;
  }

  // Return the total number of bytes.
  virtual unsigned int numBytes() const
  {
    return 0;
  }

  // Return the number of bytes per element in the container.
  virtual unsigned int numBytesPerElement() const
  {
    return 0;
  }

  // Return the number of elements in the data container.
  virtual unsigned int numElements() const
  {
    return 0;
  }

  // Return the total number of pixels.
  virtual unsigned int numPixels() const
  {
    return 0;
  }

  // Return the dimension. Photos are usually 2.
  virtual unsigned int dimensions() const
  {
    return 0;
  }

protected:

  BaseImage() : BaseClass()
  {
  }

  virtual ~BaseImage()
  {
  }
};


} // namespace Core
} // namespace Images


#endif // _IMAGES_LIBRARY_BASE_IMAGE_CLASS_H_
