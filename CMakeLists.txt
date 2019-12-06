
cmake_minimum_required( VERSION 3.1 )

include(ExternalProject)

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build."
		FORCE)
endif( NOT CMAKE_BUILD_TYPE )

SET( CMAKE_CXX_STANDARD 14 )

SET( CMAKE_CXX_STANDARD_REQUIRED ON )

project( md-pdf )

file( MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )

set( PODOFO_BUILD_LIB_ONLY ON )

add_subdirectory( 3rdparty/podofo-trunk )

add_subdirectory( md-pdf )

enable_testing()

add_subdirectory( tests )

if( ENABLE_COVERAGE )
	set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
    include( Coveralls )

	set( COVERAGE_SRCS md-pdf/md_doc.hpp
		md-pdf/md_doc.cpp
		md-pdf/md_parser.hpp
		md-pdf/md_parser.cpp )

    coveralls_setup(
        "${COVERAGE_SRCS}"
        ON )
endif()
