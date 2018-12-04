cmake_minimum_required (VERSION 2.5)

project (Helios)

set ( CMAKE_VERBOSE_MAKEFILE ON )
set ( LIBRARY_OUTPUT_PATH $ENV{HAF_LIB_DIR} )

SET ( HAF_VERSION "$ENV{HAF_VERSION_STRING}" )

set ( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" )

# Include our helper macros.
include (HafMacros)

find_package ( TBB )

SET ( BOOST_INCLUDEDIR $ENV{BOOST_INC_DIR} )
SET ( BOOST_LIBRARYDIR $ENV{BOOST_LIB_DIR} )
SET ( Boost_USE_MULTITHREADED ON )
IF(NOT MSVC)

	FIND_PACKAGE(Boost COMPONENTS date_time thread filesystem system unit_test_framework)

ELSE(NOT MSVC)

	ADD_DEFINITIONS( "-D_SCL_SECURE_NO_WARNINGS" )

	FIND_PACKAGE(Boost)
	
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ENDIF(NOT MSVC)

INCLUDE_DIRECTORIES ( 
	${Boost_INCLUDE_DIR} 
	${PROJECT_SOURCE_DIR}
	${TBB_INCLUDE_DIR} )


add_subdirectory ( Usul )
add_subdirectory ( Tree )

FIND_PACKAGE (EXPAT)

if(EXPAT_FOUND)
	add_subdirectory ( Tree/Expat )

	if (NOT BUILD_STATIC_LIBRARIES)
		add_subdirectory ( Tree/Plugins/IO/XML/Expat )
	endif()

endif()

add_subdirectory ( Tree/Registry )

find_package(SQLite)

IF(SQLITE_FOUND)
	add_subdirectory ( Database/SQLite )
ENDIF(SQLITE_FOUND)

find_package(Oracle)

IF(ORACLE_FOUND)
	add_subdirectory ( Database/Oracle )
ENDIF(ORACLE_FOUND)

find_package(PostgreSQL)

IF(POSTGRESQL_FOUND)
	add_subdirectory ( Database/PostgreSQL )
ENDIF(POSTGRESQL_FOUND)

if (NOT BUILD_STATIC_LIBRARIES)

	SET ( QT_MAX_VERSION "4.9.9" )
	SET ( QT_USE_OPENGL TRUE )
	FIND_PACKAGE(Qt4)

	IF ( QT_FOUND )
		ADD_DEFINITIONS(${QT_DEFINITIONS})
  		INCLUDE(${QT_USE_FILE})
  
  		add_subdirectory ( Tools/Qt )
		add_subdirectory ( Helios )
	ENDIF ( QT_FOUND )

endif()
