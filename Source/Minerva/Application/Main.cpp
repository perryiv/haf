
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
//  Minerva program.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Program.h"

#include "Usul/Functions/NoThrow.h"
#include "Usul/System/DateTime.h"
#include "Usul/Registry/Database.h"

#include <fstream>
#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Initialize the result.
  int result ( 1 );

  // Run the program.
  USUL_TRY_BLOCK
  {
    // Set non-persistent data in program's registry.
    Usul::Registry::Database &registry ( Usul::Registry::Database::instance(false) );
    registry["vendor"]   = "CadKit";
    registry["program"]  = "Minerva";
    registry["version"]  = Usul::System::DateTime::version ( __DATE__ );
    registry["url"]      = "www.minerva-gis.org";
    registry["icon"]     = "minerva_icon.png";
    registry["splash"]   = "minerva_splash.png";
    registry["plugins"]  = "Minerva2.plugins";
    registry["manual" ]  = "minerva.html";

    // Run the program.
    Helios::Program::run ( argc, argv );
    result = 0;
  }

  // Catch exceptions.
  USUL_DEFINE_CATCH_BLOCKS_OUTPUT_TO_STREAM ( "1630567380", std::ofstream ( "exception.txt" ) );

  // Return the result.
  return result;
}
