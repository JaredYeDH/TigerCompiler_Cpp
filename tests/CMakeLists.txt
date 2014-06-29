message( STATUS ${CMAKE_SOURCE_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${gtest_SOURCE_DIR}/include)
set( test_sources 
    CalcTest.cpp )

add_executable( AllTests
    ${test_sources})

target_link_libraries( AllTests
    Calc
    gtest_main
)
