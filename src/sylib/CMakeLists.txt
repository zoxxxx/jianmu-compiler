add_library(sylib sylib.c)
target_include_directories(sylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(
    TARGETS sylib
    ARCHIVE DESTINATION lib
)
