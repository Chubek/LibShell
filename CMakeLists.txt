cmake_minimum_required(VERSION 3.16)

project(libshell VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

option(LIBSH_BUILD_TESTS "Build LibShell test targets" ON)
option(LIBSH_BUILD_EXAMPLES "Build LibShell example targets" ON)
option(LIBSH_BUILD_STDKERN "Enable libsh_stdkern interface target" ON)
option(LIBSH_BUILD_CLI "Build LibShell CLI target" ON)
option(LIBSHELL_GENERATE_DOCS "Generate and install LibShell documentation" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    include(CTest)
endif()

if(LIBSH_BUILD_STDKERN)
    add_subdirectory(stdkern)
endif()

if(LIBSH_BUILD_CLI)
    add_subdirectory(cli)
endif()

if(LIBSH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LIBSH_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(LIBSHELL_GENERATE_DOCS)
    add_subdirectory(docs)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libshell.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libshell.pc"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libshell.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
