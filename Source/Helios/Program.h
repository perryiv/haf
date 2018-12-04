
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Derived from earlier work by Adam Kubach and Perry Miller found here:
//  http://sourceforge.net/projects/cadkit/
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios program class. Handles startup, shutdown, and branding.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_PROGRAM_H_
#define _HELIOS_PROGRAM_H_

#include "Helios/Export.h"


namespace Helios {


struct HELIOS_EXPORT Program
{
  static void run ( int argc, char **argv );
};


} // namespace Helios


#endif // _HELIOS_PROGRAM_H_
