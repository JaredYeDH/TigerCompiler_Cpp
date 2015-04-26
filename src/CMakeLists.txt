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
    Translate.cpp
)


add_executable( Tiger
    Compiler.cpp
    Main.cpp
)

target_link_libraries( Tiger
    FrontEnd
    ${Boost_LIBRARIES}
)

add_custom_command(
     TARGET Tiger
     POST_BUILD
     COMMAND AllTests
)
