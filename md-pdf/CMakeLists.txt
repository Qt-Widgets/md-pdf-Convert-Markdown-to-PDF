
project( md-pdf-gui )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC md_doc.hpp
    md_doc.cpp
    md_parser.hpp
    md_parser.cpp
	main.cpp )
    
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( md-pdf-gui ${SRC} )

target_link_libraries( md-pdf-gui Qt5::Widgets )
