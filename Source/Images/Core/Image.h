
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all templated images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_LIBRARY_TEMPLATE_IMAGE_CLASS_H_
#define _IMAGES_LIBRARY_TEMPLATE_IMAGE_CLASS_H_

#include "Images/Core/BaseImage.h"

#include "Usul/Exceptions/Exceptions.h"
#include "Usul/Strings/Format.h"

#include <algorithm>
#include <vector>


namespace Images {
namespace Core {


template < class ChannelType_ > class Image : public Images::Core::BaseImage
{
public:

  typedef ChannelType_ ChannelType;
  typedef Images::Core::BaseImage BaseClass;
  typedef Images::Core::Image < ChannelType > ThisType;
  typedef std::vector < ChannelType > Data;
  typedef typename Data::value_type ElementType;

  USUL_REFERENCED_CLASS ( Image );

  // Get the data. Use with caution.
  const Data &data() const
  {
    return _data;
  }
  Data &data()
  {
    return _data;
  }

  // Return the raw bytes.
  virtual const unsigned char *bytes() const
  {
    Guard guard ( this );
    return ( ( true == _data.empty() ) ? 0x0 : ( &_data[0] ) );
  }

  // Return the raw bytes at the given offset.
  const unsigned char *bytes ( unsigned int offset ) const
  {
    return BaseClass::bytes ( offset );
  }

  // Fill the image with the given value.
  void fill ( const ElementType &value )
  {
    std::fill ( _data.begin(), _data.end(), value );
  }

  // Return the number of bytes per element in the container.
  virtual unsigned int numBytesPerElement() const
  {
    return sizeof ( ElementType );
  }

  // Return the number of elements in the data container.
  virtual unsigned int numElements() const
  {
    Guard guard ( this );
    return _data.size();
  }

  // Return the number of bytes.
  virtual unsigned int numBytes() const
  {
    const unsigned int numElements ( this->numElements() );
    const unsigned int numBytesPerElement ( this->numBytesPerElement() );
    return ( numElements * numBytesPerElement );
  }

  // Return the number of channels.
  virtual unsigned int numChannels() const
  {
    // No need to guard because this member is const.
    return _channels;
  }

  // Return the number of pixels.
  virtual unsigned int numPixels() const
  {
    Guard guard ( this );
    const unsigned int numChannels ( this->numChannels() );
    const unsigned int numElements ( this->numElements() );
    if ( 0 != ( numElements % numChannels ) )
    {
      throw Usul::Exceptions::Error ( "8650208880", Usul::Strings::format (
        "Data vector contains '", numElements, 
        "' elements, which is not evenly divisible by '", 
        numChannels, "' channels" ) );
    }
    return ( ( 0 == numChannels ) ? 0 : ( numElements / numChannels ) );
  }

protected:

  // Construction
  Image ( unsigned int numPixels, unsigned int numChannels ) : 
    BaseClass(), _data ( numPixels * numChannels ), _channels ( numChannels )
  {
  }

  // Use reference counting.
  virtual ~Image()
  {
  }

private:

  // No default construction.
  Image();

  Data _data;
  const unsigned int _channels;
};


} // namespace Core
} // namespace Images


#endif // _IMAGES_LIBRARY_TEMPLATE_IMAGE_CLASS_H_
