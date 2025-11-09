cmake_minimum_required(VERSION 3.16)
project(llama-vulkan LANGUAGES C CXX)

# ---------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------
# Output directories per configuration (Debug/Release)
# ---------------------------------------------------------------------
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOLOWER ${OUTPUTCONFIG} OUTPUTCONFIG_LOWER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG_LOWER})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG_LOWER})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG_LOWER})
endforeach()

# ---------------------------------------------------------------------
# Includes
# ---------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------------
# Gather static libs for Debug/Release (multi-config safe)
# ---------------------------------------------------------------------
file(GLOB GGML_LIBS_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/*.lib")
file(GLOB GGML_LIBS_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/*.lib")

# ---------------------------------------------------------------------
# Ensure dynamic CRT
# ---------------------------------------------------------------------
if(MSVC)
    foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                     CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        string(REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
        string(REPLACE "/MTd" "/MDd" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

# ---------------------------------------------------------------------
# Auto-generate exports.def from llama.h
# ---------------------------------------------------------------------
set(EXPORT_DEF "${CMAKE_CURRENT_SOURCE_DIR}/src/exports.def")

# Example generation command (optional, if you really generate from header)
# add_custom_command(
#     OUTPUT ${EXPORT_DEF}
#     COMMAND ${CMAKE_COMMAND} -E echo "LIBRARY llama-vulkan" > ${EXPORT_DEF}
#     COMMAND ${CMAKE_COMMAND} -E echo "EXPORTS" >> ${EXPORT_DEF}
#     VERBATIM
# )

# ---------------------------------------------------------------------
# Shared library target
# ---------------------------------------------------------------------
add_library(llama-vulkan SHARED
    src/llama-vulkan.cpp
    ${EXPORT_DEF}
)

set_target_properties(llama-vulkan PROPERTIES
    OUTPUT_NAME "llama-vulkan"
    LINK_FLAGS "/DEF:${EXPORT_DEF}"
)

# ---------------------------------------------------------------------
# Link all static libs (Debug/Release)
# ---------------------------------------------------------------------
target_link_libraries(llama-vulkan PRIVATE
    $<$<CONFIG:Debug>:${GGML_LIBS_DEBUG}>
    $<$<CONFIG:Release>:${GGML_LIBS_RELEASE}>
)

find_package(Vulkan REQUIRED)
target_link_libraries(llama-vulkan PRIVATE
	Vulkan::Vulkan
)

# ---------------------------------------------------------------------
# Optional defines for compatibility
# ---------------------------------------------------------------------
target_compile_definitions(llama-vulkan PRIVATE
    GGML_SHARED
    GGML_BUILD
    GGML_BACKEND_SHARED
    GGML_BACKEND_BUILD
)

# ---------------------------------------------------------------------
# Post build message
# ---------------------------------------------------------------------
add_custom_command(TARGET llama-vulkan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Built llama-vulkan.dll + llama-vulkan.lib"
)

# ---------------------------------------------------------------------
# Test executable
# ---------------------------------------------------------------------
add_executable(llama_test
    src/test_main.cpp
)

target_link_libraries(llama_test PRIVATE llama-vulkan)

# ---------------------------------------------------------------------
# Copy DLL beside test executable
# ---------------------------------------------------------------------
add_custom_command(TARGET llama_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:llama-vulkan>
        $<TARGET_FILE_DIR:llama_test>
)
