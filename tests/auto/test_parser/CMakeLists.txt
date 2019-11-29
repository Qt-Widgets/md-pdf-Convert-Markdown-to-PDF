
project( test.md_parser )

find_package( Qt5 COMPONENTS Core REQUIRED )

set( SRC main.cpp )

file( COPY test1.md
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY test2.md
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY test3.md
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY test4.md
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY test5.md
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
    
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../..
	${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty )

link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib )

add_executable( test.md_parser ${SRC} )

if( ENABLE_COVERAGE )
	target_compile_options( test.md_parser PRIVATE --coverage )
	target_link_libraries( test.md_parser PRIVATE --coverage md-parser Qt5::Core )
else()
	target_link_libraries( test.md_parser md-parser Qt5::Core )
endif( ENABLE_COVERAGE )

add_test( NAME test.md_parser
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test.md_parser
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
