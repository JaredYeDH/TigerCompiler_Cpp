add_library( FrontEnd
	Tokens.cpp
    TokenStream.cpp
    Position.cpp
    PositionCountingStream.cpp
    Lexer.cpp
    AST.cpp
    Parser.cpp
    Symbol.cpp
    Types.cpp
    Environments.cpp
    CompileTimeErrorHandler.cpp
    ErrorCodes.cpp
    EscapeCalculator.cpp
    Temps.cpp
    Platform_Specific/x86/Frame_x86.cpp # TODO: Fix this makefile
)


add_executable( Main
    Main.cpp
)

target_link_libraries( Main
    FrontEnd
    ${Boost_LIBRARIES}
)

add_custom_command(
     TARGET Main
     POST_BUILD
     COMMAND AllTests
)
