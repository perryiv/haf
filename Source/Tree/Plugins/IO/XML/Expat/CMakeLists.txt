set ( header_files
./TreePluginExpat.cpp
)

set ( source_files
./CompileGuard.h
./TreePluginExpat.h
)

# Add the Export.def file.
IF ( MSVC )
	SET ( source_files ${source_files} Export.def )
ENDIF ( MSVC )

set ( plugin_name TreeRegistryPlugin )

add_definitions ( "-D_COMPILING_TREE_IO_XML_EXPAT" )

add_library ( ${plugin_name} SHARED ${header_files} ${source_files} )

HAF_SETUP_PLUGIN ( ${plugin_name} )

target_link_libraries ( ${plugin_name} Usul Tree TreeExpat )
