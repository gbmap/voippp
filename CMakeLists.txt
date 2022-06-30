cmake_minimum_required(VERSION 3.0.0)
project(voippp VERSION 0.1.0)

enable_testing()

find_package(SDL2 REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(Boost REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_executable(voippp src/main.cpp src/buffer_fill.cpp src/network.cpp)
add_executable(buffer_fill_tests src/buffer_fill.cpp tests/tests_buffer_fill.cpp)


target_link_libraries(voippp SDL2::SDL2 Boost::boost)
target_link_libraries(buffer_fill_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
include(CTest)
include(Catch)
catch_discover_tests(buffer_fill_tests)
