add_library( Lexer
	Tokens.cpp
    TokenStream.cpp
    Lexer.cpp
)

add_library ( Parser
    Parser.cpp
    Symbol.cpp
)

add_executable( Main
    Main.cpp
)

target_link_libraries( Main
    Lexer
    Parser
    ${Boost_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_custom_command(
     TARGET Main
     POST_BUILD
     COMMAND AllTests
)
