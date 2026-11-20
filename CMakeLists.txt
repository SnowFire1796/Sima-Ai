cmake_minimum_required(VERSION 3.20)
project(SimaAI VERSION 0.1.0 LANGUAGES CXX)
option(SIMA_BUILD_TESTS "Build tests" ON)
add_library(sima_core
    src/text.cpp
    src/storage.cpp
    src/dataset.cpp
    src/learning.cpp
    src/reasoner.cpp
    src/retrieval.cpp
    src/generation.cpp
    src/evaluation.cpp
    src/memory.cpp
    src/engine.cpp
)
target_include_directories(sima_core PUBLIC include)
target_compile_features(sima_core PUBLIC cxx_std_20)
set_target_properties(sima_core PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
    target_compile_options(sima_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(sima_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
add_executable(sima src/main.cpp)
target_link_libraries(sima PRIVATE sima_core)
if(SIMA_BUILD_TESTS)
    enable_testing()
    add_executable(sima_tests tests/test_main.cpp)
    target_link_libraries(sima_tests PRIVATE sima_core)
    add_test(NAME sima_tests COMMAND sima_tests)
endif()
install(TARGETS sima RUNTIME DESTINATION bin)
