cmake_minimum_required(VERSION 3.16.0)
project(LINDA VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fetching GoogleTest release 1.11.0 from github repo
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         e2239ee6043f73722e7aa812a459f54a28552929
)
FetchContent_MakeAvailable(googletest)

file(GLOB LIB_SRC "./src/*.cpp" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
# Output directory for shared library target
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
# Output directory for static library target
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(LIB_NAME "linda")
add_library(${LIB_NAME} SHARED ${LIB_SRC})

target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_HOME_DIRECTORY}/include)
if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -pedantic -O0)
else()
    target_compile_options(${LIB_NAME} PRIVATE -Ofast)
endif()

set(BUILD_EXAMPLES False)
set(BUILD_TESTS True)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if(BUILD_TESTS)
    enable_testing()
    message("Building tests.")
    file(GLOB TEST_SRC "./test/*.cpp" )

    set(TEST_TARGET "UnitTests")

    add_executable(${TEST_TARGET} ${TEST_SRC})
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_HOME_DIRECTORY}/include)
    target_link_libraries(${TEST_TARGET} PRIVATE gtest_main ${LIB_NAME})

    #Discover test with CMake runner
    include(GoogleTest)
    gtest_discover_tests(${TEST_TARGET})
endif()