
set ( HEADERS
./Application.h
./ChildWindows/ViewContainer.h
./Commands/Update.h
./Export.h
./MainWindows/MainWindow.h
./MainWindows/MainWindowMDI.h
./MainWindows/MainWindowSDI.h
./Menus/Action.h
./Menus/Builder.h
./Menus/Button.h
./Menus/Functors.h
./Menus/Item.h
./Menus/Menu.h
./Menus/MenuBar.h
./Menus/Separator.h
./Menus/ToolBar.h
./Menus/Visitor.h
./OutputFile.h
./Program.h
)

set ( SOURCES
./Application.cpp
./ChildWindows/ViewContainer.cpp
./MainWindows/MainWindow.cpp
./MainWindows/MainWindowMDI.cpp
./MainWindows/MainWindowSDI.cpp
./Menus/Action.cpp
./Menus/Builder.cpp
./Menus/Button.cpp
./Menus/Item.cpp
./Menus/Menu.cpp
./Menus/MenuBar.cpp
./Menus/Separator.cpp
./Menus/ToolBar.cpp
./Menus/Visitor.cpp
./Program.cpp
)

SET(MOC_HDRS
  ./MainWindows/MainWindow.h
  ./ChildWindows/ViewContainer.h
  ./Menus/Action.h
)

QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

add_library ( Helios SHARED ${HEADERS} ${SOURCES} ${MOC_SRCS} )

HAF_SETUP_LIBRARY ( Helios )

TARGET_LINK_LIBRARIES( Helios Usul QtTools TreeRegistry
   ${QT_LIBRARIES}
)
