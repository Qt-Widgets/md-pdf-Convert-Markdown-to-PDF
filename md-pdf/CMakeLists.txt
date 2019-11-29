
project( md-pdf-gui )

if( ENABLE_COVERAGE )
	set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake )
    include( Coveralls )
    coveralls_turn_on_coverage()
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

set( GUI_SRC main.cpp )
    
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib )

add_library( md-parser STATIC ${LIB_SRC} )

target_link_libraries( md-parser Qt5::Core )

add_executable( md-pdf-gui ${GUI_SRC} )

target_link_libraries( md-pdf-gui md-parser Qt5::Widgets )
