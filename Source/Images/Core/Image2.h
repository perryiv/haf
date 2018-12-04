
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  2D image class. Intentionally not thread safe for speed.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_LIBRARY_2D_IMAGE_CLASS_H_
#define _IMAGES_LIBRARY_2D_IMAGE_CLASS_H_

#include "Images/Core/Image.h"


namespace Images {
namespace Core {


template < class ChannelType_ > class Image2 : 
  public Images::Core::Image < ChannelType_ >
{
public:

  typedef ChannelType_ ChannelType;
  typedef Images::Core::Image < ChannelType > BaseClass;
  typedef Images::Core::Image2 < ChannelType > ThisType;

  USUL_REFERENCED_CLASS ( Image2 );

  // Construction
  Image2 ( unsigned int width, unsigned int height, unsigned int channels ) : 
    BaseClass ( width * height, channels ),
    _width ( width )
  {
  }

  // Get the size.
  unsigned int width() const
  {
    return _width;
  }
  unsigned int height() const
  {
    const unsigned int numChannels ( this ->numChannels() );
    const unsigned int wc ( _width * numChannels );
    if ( 0 == wc )
    {
      return 0;
    }
    const unsigned int numElements ( this->numElements() );
    if ( 0 != ( numElements % wc ) )
    {
      throw Usul::Exceptions::Error ( "2276318415", Usul::Strings::format (
        "Data vector contains '", numElements, 
        "' elements, which is not evenly divisible by '", wc, 
        "', which is the width '", _width, 
        "' times the number of channels '", numChannels, "'" ) );
    }
    return ( numElements / wc );
  }

  // Return the dimensions.
  virtual unsigned int dimensions() const
  {
    return 2;
  }

protected:

  // Use reference counting.
  virtual ~Image2()
  {
  }

private:

  const unsigned int _width;
};


} // namespace Core
} // namespace Images


#endif // _IMAGES_LIBRARY_2D_IMAGE_CLASS_H_
