
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time values in seconds or milliseconds.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Clock.h"

#include <ctime>

#ifndef _MSC_VER
# include <sys/time.h> // For gettimeofday
#endif 


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the clock internals.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      Usul::System::Clock::milliseconds();
    }
  } init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the milliseconds offset from a platform dependent value.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::UInt64 Usul::System::Clock::milliseconds() 
{
#ifdef _MSC_VER
  return ::clock();
#else
  struct timeval t1;
  gettimeofday(&t1, NULL);
  Usul::Types::UInt64 seconds ( t1.tv_sec );
  Usul::Types::UInt64 microSec ( t1.tv_usec );
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the seconds offset from a platform dependent value
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::UInt64 Usul::System::Clock::seconds() 
{
#ifdef _MSC_VER
  return ::clock()/1000;
#else
  struct timeval t1;
  gettimeofday(&t1, NULL);
  return t1.tv_sec;
#endif
}
