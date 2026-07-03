add_library(libsh_stdkern INTERFACE)
target_include_directories(libsh_stdkern INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../include"
)
target_compile_features(libsh_stdkern INTERFACE cxx_std_20)
