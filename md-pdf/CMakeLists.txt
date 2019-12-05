
project( md-pdf-gui )

if( ENABLE_COVERAGE )
	set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake )
    include( Coveralls )
    coveralls_turn_on_coverage()
endif()

if( ENABLE_COVERAGE )
	set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
    include( Coveralls )
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( LIB_SRC md_doc.hpp
    md_doc.cpp
    md_parser.hpp
    md_parser.cpp )

set( GUI_SRC main.cpp
	main_window.cpp
	main_window.hpp
	main_window.ui
	color_widget.hpp
	color_widget.cpp
	renderer.hpp
	renderer.cpp )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/podofo/include )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../lib/podofo/lib )

find_package( Threads )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib )

add_library( md-parser STATIC ${LIB_SRC} )

target_link_libraries( md-parser Qt5::Core )

add_executable( md-pdf-gui ${GUI_SRC} )

if( WIN32 )
	target_link_libraries( md-pdf-gui md-parser podofo freetype
		fontconfig jpeg z Qt5::Widgets gdi32 ws_win32 )
else()
	target_link_libraries( md-pdf-gui md-parser podofo freetype
		fontconfig jpeg z Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT} )
endif()
