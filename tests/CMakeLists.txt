message( STATUS ${CMAKE_SOURCE_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${gtest_SOURCE_DIR}/include)

set( test_sources 
    ASTTest.cpp 
    TokensTest.cpp
    TokenStreamTest.cpp
    LexerTest.cpp
    ParserTest.cpp
    PositionCountingStreamTest.cpp
    SymbolTest.cpp
    SymbolTableTest.cpp
    TypeTest.cpp
    EnvironmentsTest.cpp
    EndToEndTests.cpp
    EscapeCalculatorTest.cpp
)

add_executable( AllTests
    ${test_sources})

target_link_libraries( AllTests
    FrontEnd
    gtest_main
    ${Boost_LIBRARIES}
)

add_custom_command(
    TARGET AllTests
    POST_BUILD
    COMMAND AllTests
)
