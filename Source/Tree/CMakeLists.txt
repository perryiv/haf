set ( header_files
./Export.h
./Find.h
./Functions.h
./Node.h
./Predicates.h
./Xerces/Convert.h
./IO/ReadRVHO.h
./IO/WriteRVHO.h
./IO/WriteXML.h
./Interfaces/IReadXML.h
)

set ( source_files
./Functions.cpp
./Node.cpp
./IO/ReadRVHO.cpp
./IO/WriteRVHO.cpp
./IO/WriteXML.cpp
)

include_directories ( ../ )

set ( TARGET_NAME Tree )
if (BUILD_STATIC_LIBRARIES)
	set ( TARGET_NAME ${TARGET_NAME}${HAF_STATIC_SUFFIX} )
endif()

add_library ( ${TARGET_NAME} ${LIBRARY_TYPE} ${header_files} ${source_files} )

HAF_SETUP_LIBRARY ( ${TARGET_NAME} )

target_link_libraries ( ${TARGET_NAME} Usul )
