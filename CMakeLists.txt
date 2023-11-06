cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(hw2 LANGUAGES CXX)

include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF." FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Build Release candidates with -Werror." FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable benchmark unit tests" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Enable benchmark unit tests which depend on gtest" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Enable installation of benchmark" FORCE)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.2
)
FetchContent_MakeAvailable(benchmark)

function(addTask source_file)
    file(GLOB_RECURSE utils_sources ./utils/**.h ./utils/**.cpp)
    file(GLOB_RECURSE headers ./*.h)

    get_filename_component(executable_name ${source_file} NAME_WLE)

    add_executable(${executable_name} ${source_file} ${utils_sources} ${headers} "utils/benchmarkdata.h" "utils/common_impl.h" "utils/internalerror.h" "utils/kth-order-statistics.h" "utils/main.h" "utils/median.h" "utils/min-max-element.h" "utils/quicksort.h")
    set_target_properties(${executable_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    target_link_libraries(${executable_name} PRIVATE gtest gtest_main benchmark::benchmark)
    target_include_directories(${executable_name} PRIVATE .)

    target_compile_definitions(${executable_name} PRIVATE HW2_BENCHMARKS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")
    target_compile_definitions(${executable_name} PRIVATE HW2_EXECUTABLE_NAME="${executable_name}")
endfunction()

addTask(task2-a-min-max-element.cpp)
addTask(task2-b-median.cpp)
addTask(task2-c-kth-order-statistics.cpp)
addTask(task3-quicksort.cpp)
