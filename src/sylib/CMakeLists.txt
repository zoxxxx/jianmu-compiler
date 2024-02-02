add_library(sylib sylib.c)

install(
    TARGETS sylib
    ARCHIVE DESTINATION lib
)
