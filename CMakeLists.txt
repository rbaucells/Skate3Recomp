cmake_minimum_required(VERSION 4.2)
project(Skate3Recomp)

set(CMAKE_CXX_STANDARD 20)

# xenon and xenos recomp
add_subdirectory(XenonRecomp-Skate-3)
add_subdirectory(XenosRecomp-Skate-3)

# get skate 3 ppc files locations
set(SKATE3_PPC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/ppc_config.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/ppc_context.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/ppc_func_mapping.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/ppc_recomp_shared.h"
)

foreach(i RANGE 0 275)
    list(APPEND SKATE3_PPC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/ppc_recomp.${i}.cpp")
endforeach()

# generate them if they dont exist
add_custom_command(
        OUTPUT ${SKATE3_PPC_FILES}
        COMMAND "$<TARGET_FILE:XenonRecomp>" "${CMAKE_CURRENT_SOURCE_DIR}/config/toml.toml" "${CMAKE_CURRENT_SOURCE_DIR}/XenonRecomp-Skate-3/XenonUtils/ppc_context.h"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/skate-3/default.xex"
        COMMENT "Generating skate 3 ppc files"
        USES_TERMINAL
)

add_executable(Skate3Recomp main.cpp ${SKATE3_PPC_FILES})

#target_include_directories(Skate3Recomp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Skate3Recomp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/XenonRecomp-Skate-3/thirdparty/simde")
target_precompile_headers(Skate3Recomp PUBLIC "out/ppc/ppc_recomp_shared.h")
