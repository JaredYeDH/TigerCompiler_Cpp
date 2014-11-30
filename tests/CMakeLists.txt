message( STATUS ${CMAKE_SOURCE_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${gtest_SOURCE_DIR}/include)
set( test_sources 
    ASTTest.cpp 
    TokensTest.cpp
    LexerTest.cpp)

add_executable( AllTests
    ${test_sources})

target_link_libraries( AllTests
    AST
    Lexer
    gtest_main
)

add_custom_command(
    TARGET AllTests
    POST_BUILD
    COMMAND AllTests)
