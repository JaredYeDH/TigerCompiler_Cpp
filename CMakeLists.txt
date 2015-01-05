cmake_minimum_required( VERSION 2.6 )
project( ProjectName )

add_definitions( -std=c++11 -g -Wall)

# Set BOOST_ROOT environment variable to point to your local Boost dir.
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )

# ensure that we have gtest
execute_process( COMMAND "../scripts/SetupGtest.sh" )

add_subdirectory( src )
add_subdirectory( gtest-1.7.0 )
add_subdirectory( tests )

enable_testing()
add_test(test
        tests/AllTests)

add_custom_target(loop
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/LoopBuild.sh)
