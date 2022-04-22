cmake_minimum_required(VERSION 3.16.0)
project(LINDA VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_library(LINDA linda
        HINTS ${CMAKE_HOME_DIRECTORY}/lib/
        REQUIRED
)

# Add src folder for example and set example target name
file(GLOB EXAMPLE_SRC "./example/*.cpp" )
set(EXAMPLE_EXAMPLE example)
set(ACCEPTANCE_TEST_1 test-1)
set(ACCEPTANCE_TEST_2 test-2)
set(ACCEPTANCE_TEST_3 test-3)

# Add include folders
include_directories(${CMAKE_HOME_DIRECTORY}/include)

# Add executable for example
add_executable(${EXAMPLE_EXAMPLE} ${EXAMPLE_SRC})
target_link_libraries(${EXAMPLE_EXAMPLE} PRIVATE ${LINDA})

add_executable(${ACCEPTANCE_TEST_1} "./acceptance_tests/test1.cpp")
target_link_libraries(${ACCEPTANCE_TEST_1} PRIVATE ${LINDA})

add_executable(${ACCEPTANCE_TEST_2} "./acceptance_tests/test2.cpp")
target_link_libraries(${ACCEPTANCE_TEST_2} PRIVATE ${LINDA})

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  target_compile_options(${EXAMPLE_EXAMPLE} PRIVATE -Wall -Wextra -pedantic -O0)
  target_compile_options(${ACCEPTANCE_TEST_1} PRIVATE -Wall -Wextra -pedantic -O0)
  target_compile_options(${ACCEPTANCE_TEST_2} PRIVATE -Wall -Wextra -pedantic -O0)
else()
  target_compile_options(${EXAMPLE_EXAMPLE} PRIVATE -Ofast)
  target_compile_options(${ACCEPTANCE_TEST_1} PRIVATE -Ofast)
  target_compile_options(${ACCEPTANCE_TEST_2} PRIVATE -Ofast)
endif()
