set ( header_files
./Export.h
./Functions.h
./Lock.h
./Transaction.h
./Connection.h
./Result.h
./Exception.h
)

set ( source_files
./Functions.cpp
./Lock.cpp
./Result.cpp
./Connection.cpp
./Exception.cpp
./Transaction.cpp
)

include_directories ( ../../ /usr/include/postgresql ${POSTGRESQL_INCLUDE_DIR} )

add_definitions ( "-D_COMPILING_DATABASE_POSTGRESQL" )

set ( TARGET_NAME Database_PostgreSQL )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Usul ${POSTGRESQL_LIBRARIES} )
