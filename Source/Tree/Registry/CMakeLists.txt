set ( header_files
./Builder.h
./Export.h
./IO.h
./Visitor.h
)

set ( source_files
./Builder.cpp
./IO.cpp
./Visitor.cpp
)

include_directories ( ../../ )

set ( TARGET_NAME TreeRegistry )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Usul Tree )
