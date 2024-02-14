cmake_minimum_required(VERSION 3.22)

project(lifs_main)

set(CMAKE_CXX_STANDARD 20)

include_directories(liblifs)
add_subdirectory(liblifs)

aux_source_directory(./lifs/ DIR_SRC)

add_executable(lifs_main ${DIR_SRC})
target_link_libraries(lifs_main lifs)