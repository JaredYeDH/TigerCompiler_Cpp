add_library( AST
    AST.cpp
)

add_library( Lexer
	Tokens.cpp
    Lexer.cpp
)

add_executable( Main
    Main.cpp
)

target_link_libraries( Main
    AST
    Lexer
    ${Boost_LIBRARIES}
)

add_custom_command(
     TARGET Main
     POST_BUILD
     COMMAND AllTests)
