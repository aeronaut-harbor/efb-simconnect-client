cmake_minimum_required(VERSION 3.24)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(EFBSimConnectClient)

add_executable(EFBSimConnectClient
    src/main.cpp
)

target_include_directories(EFBSimConnectClient PRIVATE
    $ENV{MSFS_SDK}
)

target_compile_features(EFBSimConnectClient PRIVATE cxx_std_23)

target_compile_options(EFBSimConnectClient PRIVATE
    /W4 
    /WX
)

target_link_libraries(EFBSimConnectClient PRIVATE
    "$ENV{MSFS_SDK}/SimConnect SDK/lib/SimConnect.lib"
)

target_link_options(EFBSimConnectClient PRIVATE
    /INCREMENTAL:NO
)

add_custom_command(TARGET EFBSimConnectClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$ENV{MSFS_SDK}/SimConnect SDK/lib/SimConnect.dll"
        $<TARGET_FILE_DIR:EFBSimConnectClient>
)
