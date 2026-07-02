# Runtime test suite for the LibShell/Lakposht runtime.
#
# Self-contained CTest project. Build and run on its own:
#   cmake -S tests -B tests/build
#   cmake --build tests/build
#   ctest --test-dir tests/build --output-on-failure
#
# Or consume from a parent project with add_subdirectory(tests). The suite
# links only the in-tree sources: test_runtime.cpp plus the CLI parser TU it
# depends on. No external test framework is required.

cmake_minimum_required(VERSION 3.16)

if(NOT PROJECT_NAME)
    project(libsh_tests LANGUAGES CXX)
endif()

enable_testing()

set(LIBSH_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

add_executable(test_runtime
    test_runtime.cpp
    "${LIBSH_ROOT}/cli/libsh-parse.cpp"
)

target_include_directories(test_runtime PRIVATE "${LIBSH_ROOT}/include")
target_compile_features(test_runtime PRIVATE cxx_std_20)
target_compile_options(test_runtime PRIVATE -Wall -Wextra -Werror -pedantic)

# The suite forks real child processes (fork/execvp), so give it a working
# directory under the build tree rather than scattering temp files.
add_test(NAME runtime COMMAND test_runtime WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
