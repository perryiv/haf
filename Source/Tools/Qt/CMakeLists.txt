
SET ( HEADERS
./Builder.h
./Export.h
./FileDialog.h
./Functions.h
./Image.h

)

SET ( SOURCES 
./Builder.cpp
./FileDialog.cpp
./Functions.cpp
./Image.cpp
)

add_library ( QtTools SHARED ${HEADERS} ${SOURCES} )

TARGET_LINK_LIBRARIES( QtTools Usul ${QT_LIBRARIES} ${QT_QTUITOOLS_LIBRARY} )
