add_library( Lexer
	Tokens.cpp
    TokenStream.cpp
    Position.cpp
    PositionCountingStream.cpp
    Lexer.cpp
)

add_library ( Parser
    AST.cpp
    Parser.cpp
    Symbol.cpp
    Types.cpp
    Environments.cpp
)

add_executable( Main
    Main.cpp
)

target_link_libraries( Main
    Lexer
    Parser
    ${Boost_LIBRARIES}
)

add_custom_command(
     TARGET Main
     POST_BUILD
     COMMAND AllTests
)
