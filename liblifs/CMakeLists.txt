add_library(
        lifs
        lifs.cpp
        arbiter.cpp
        arbiter.h
        storage.cpp
        storage.h
        dictionary.cpp
        dictionary.h
)
find_package(mio REQUIRED)
target_link_libraries(lifs PUBLIC mio::mio)

# --------------------GTEST--------------------
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif ()
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_storage test/test_storage.cpp)
target_link_libraries(test_storage GTest::gtest_main lifs)

add_executable(test_dictionary test/test_dictionary.cpp)
target_link_libraries(test_dictionary GTest::gtest_main lifs)

add_executable(test_arbiter test/test_arbiter.cpp)
target_link_libraries(test_arbiter GTest::gtest_main lifs)

include(GoogleTest)
gtest_discover_tests(test_storage test_dictionary)

