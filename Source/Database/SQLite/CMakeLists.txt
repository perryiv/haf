
set ( header_files
./Export.h
./Transaction.h
./Binder.h
./Connection.h
./Internal.h
./Result.h
./Types.h
./Input.h
)

set ( source_files
./Result.cpp
./Connection.cpp
./Binder.cpp
)

include_directories ( ../../ )

add_definitions ( "-D_COMPILING_DATABASE_SQLITE" )

set ( TARGET_NAME Database_SQLite )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif ()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Usul ${SQLITE_LIBRARY} )
