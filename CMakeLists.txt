cmake_minimum_required( VERSION 2.8.12 )

project( lib_nodepp_prj )

find_package( Boost 1.61.0 COMPONENTS system filesystem iostreams regex unit_test_framework REQUIRED )

set( CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard whose features are requested.")
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

include_directories( "./include" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
if( EXISTS "${DEP_ROOT_DIR}" )
	message( "${DEP_ROOT_DIR}" )
	message( "using dependency folder" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${DEP_ROOT_DIR}/install/include" )
	link_directories( "${DEP_ROOT_DIR}/install/lib" )
else( )
	message( "downloading and building deps" )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/static_fmt.h
)

set( SOURCE_FILES
)

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

add_executable( static_fmt_test_bin ${HEADER_FILES} ${TEST_FOLDER}/static_fmt_test.cpp )
target_link_libraries( static_fmt_test_bin ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_test( test_static_fmt static_fmt_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/ )

