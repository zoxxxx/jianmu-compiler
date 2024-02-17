add_library(common STATIC
    syntax_tree.c
    ast.cpp
    logging.cpp
    get_constnum.cpp
)

target_link_libraries(common)

