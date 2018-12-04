set ( header_files
./Export.h
./Environment.h
./Execute.h
./Connection.h
./Result.h
./Types.h
)

set ( source_files
./Result.cpp
./Execute.cpp
./Connection.cpp
./Environment.cpp
)

include_directories ( ../../ ${ORACLE_INCLUDE_DIR} )

add_definitions ( "-D_COMPILING_DATABASE_ORACLE" )

set ( TARGET_NAME Database_Oracle )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Usul ${ORACLE_LIBRARIES} )
