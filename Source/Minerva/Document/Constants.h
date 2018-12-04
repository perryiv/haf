
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_DOCUMENT_CONSTANTS_H_
#define _MINERVA_DOCUMENT_CONSTANTS_H_

#include "Minerva/Document/CompileGuard.h"

#include "Usul/Types/Types.h"


namespace Minerva
{
  namespace Constants
  {
    const double WGS_84_RADIUS_EQUATOR  ( 6378137.0 );
    const double WGS_84_RADIUS_POLAR    ( 6356752.3142 );

    const Usul::Types::UInt64 TARGET_FRAME_RATE ( 30 );
    const Usul::Types::UInt64 UPDATE_TIME_ALLOWED ( 500 );
  };
}


#endif // _MINERVA_DOCUMENT_CONSTANTS_H_
