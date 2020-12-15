
include_directories(
    .
)

aux_source_directory(. pystring)
add_library(pystring ${pystring})

add_subdirectory(test)