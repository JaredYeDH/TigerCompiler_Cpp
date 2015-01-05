message( STATUS ${CMAKE_SOURCE_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${gtest_SOURCE_DIR}/include)

set( test_sources 
    ASTTest.cpp 
    TokensTest.cpp
    TokenStreamTest.cpp
    LexerTest.cpp
    ParserTest.cpp
    SymbolTest.cpp
    EndToEndTests.cpp
)

add_executable( AllTests
    ${test_sources})

target_link_libraries( AllTests
    Lexer
    Parser
    gtest_main
    ${Boost_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_custom_command(
    TARGET AllTests
    POST_BUILD
    COMMAND AllTests
)
