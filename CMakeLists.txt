cmake_minimum_required(VERSION 4.2)
project(Skate3Recomp)

if(NOT VCPKG_ROOT)
    set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/thirdparty/vcpkg")
endif()
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
endif()

set(CMAKE_CXX_STANDARD 20)

set(SKATE3_RECOMP_TOOLS "${CMAKE_SOURCE_DIR}/tools")
set(SKATE3_RECOMP_THIRDPARTY "${CMAKE_SOURCE_DIR}/thirdparty")
set(SKATE3_RECOMP_LIB "${CMAKE_SOURCE_DIR}/Skate3RecompLib")
set(SKATE3_RECOMP_CONFIG "${CMAKE_SOURCE_DIR}/config")
set(SKATE3_RECOMP_OUT "${CMAKE_SOURCE_DIR}/out")

if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif ()

# thirdparty dependencies
add_subdirectory(thirdparty)

# tools
add_subdirectory(tools)

# main library
add_subdirectory(Skate3RecompLib)

# main executable
add_subdirectory(Skate3Recomp)