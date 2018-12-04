
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for writing and returning a new image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_LIBRARY_INTERFACES_WRITE_H_
#define _IMAGES_LIBRARY_INTERFACES_WRITE_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Images/Core/BaseImage.h"

#include <string>


namespace Images {
namespace Interfaces {


struct IImageWrite : public Usul::Interfaces::IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IImageWrite );
  typedef Images::Core::BaseImage::RefPtr ImagePtr;
  enum { IID = 3462528887u };

  // Can we write the image file?
  virtual bool          canWriteImageFile ( ImagePtr image, const std::string &file ) = 0;

  // Write the image to file.
  virtual void          writeImageFile ( 
                          ImagePtr image,
                          const std::string &file, 
                          Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


} // End namespace Interfaces
} // End namespace Images


#endif // _IMAGES_LIBRARY_INTERFACES_WRITE_H_
