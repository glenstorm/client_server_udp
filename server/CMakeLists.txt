cmake_minimum_required( VERSION 3.9 )
set( PROJECT_NAME udp_server )
project( ${PROJECT_NAME} )


set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -rdynamic" )
set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -rdynamic -s" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -rdynamic")
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -rdynamic" )

if( UNIX )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
	set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17")
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17" )
	set( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined" )
endif( UNIX )

# sources to include in project
set( SOURCE
main.cpp
)

set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")

include_directories(
${PROJECT_SOURCE_DIR}
)

#turn on ccache if installed
find_program( CCACHE_FOUND ccache )
if( CCACHE_FOUND )
    set_property( GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache )
    set_property( GLOBAL PROPERTY RULE_LAUNCH_LINK ccache )
endif( CCACHE_FOUND )

##########################################################
add_executable( ${PROJECT_NAME} ${SOURCE} )

# target_link_libraries( 	${PROJECT_NAME} )
