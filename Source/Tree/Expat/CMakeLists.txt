
set ( header_files
./Export.h
./Reader.h
)

set ( source_files
./Reader.cpp
)

include_directories ( ../../ ${EXPAT_INCLUDE_DIR} )

set ( TARGET_NAME TreeExpat )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Tree ${EXPAT_LIBRARY} ${Boost_LIBRARIES} )
