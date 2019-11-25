
project( test.md_parser )

if( ENABLE_COVERAGE )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage" )
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage" )
endif( ENABLE_COVERAGE )

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
	${CMAKE_CURRENT_SOURCE_DIR}/../../../md-pdf
	${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty )

link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib )

add_executable( test.md_parser ${SRC} )

target_link_libraries( test.md_parser md-parser Qt5::Core )

add_test( NAME test.md_parser
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test.md_parser
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
