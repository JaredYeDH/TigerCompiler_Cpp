add_library( Calc
    Calc.cpp
)

add_executable( Main
    Main.cpp
)

target_link_libraries( Main
    Calc
    ${Boost_LIBRARIES}
)
