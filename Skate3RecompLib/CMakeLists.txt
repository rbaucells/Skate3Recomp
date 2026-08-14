project("Skate3RecompLib")

add_compile_options(-fno-strict-aliasing)

if (WIN32)
    add_compile_options(/fp:strict)
else()
    add_compile_options(-ffp-model=strict)
endif()

set(SKATE3_RECOMP_PPC_DEFAULT_RECOMPILED_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/default_ppc/ppc_config.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/default_ppc/ppc_context.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/default_ppc/ppc_func_mapping.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/default_ppc/ppc_recomp_shared.h"
)

foreach(i RANGE 0 275)
    list(APPEND SKATE3_RECOMP_PPC_DEFAULT_RECOMPILED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/default_ppc/ppc_recomp.${i}.cpp")
endforeach()

set(SKATE3_RECOMP_PPC_EAWEBKIT_RECOMPILED_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/eawebkit_ppc/ppc_config.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/eawebkit_ppc/ppc_context.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/eawebkit_ppc/ppc_func_mapping.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/eawebkit_ppc/ppc_recomp_shared.h"
)

foreach(i RANGE 0 119)
    list(APPEND SKATE3_RECOMP_PPC_EAWEBKIT_RECOMPILED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/out/ppc/eawebkit_ppc/ppc_recomp.${i}.cpp")
endforeach()

add_custom_command(
        OUTPUT
            ${SKATE3_RECOMP_PPC_DEFAULT_RECOMPILED_SOURCES}
        COMMAND
            $<TARGET_FILE:XenonRecomp> "${CMAKE_CURRENT_SOURCE_DIR}/config/default_toml.toml" "${SKATE3_RECOMP_TOOLS}/XenonRecomp-Skate-3/XenonUtils/ppc_context.h"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/skate-3/default_patched.xex"
            "${CMAKE_CURRENT_SOURCE_DIR}/config/default_toml.toml"
        USES_TERMINAL
)

add_custom_command(
        OUTPUT
            ${SKATE3_RECOMP_PPC_EAWEBKIT_RECOMPILED_SOURCES}
        COMMAND
            $<TARGET_FILE:XenonRecomp> "${CMAKE_CURRENT_SOURCE_DIR}/config/eawebkit_toml.toml" "${SKATE3_RECOMP_TOOLS}/XenonRecomp-Skate-3/XenonUtils/ppc_context.h"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/skate-3/eawebkit_patched.xex"
            "${CMAKE_CURRENT_SOURCE_DIR}/config/eawebkit_toml.toml"
        USES_TERMINAL
)

target_compile_definitions(XenosRecomp PRIVATE
        XENOS_RECOMP_INPUT=\"${CMAKE_CURRENT_SOURCE_DIR}/skate-3\"
        XENOS_RECOMP_OUTPUT=\"${CMAKE_CURRENT_SOURCE_DIR}/out/shaders/shader_cache.cpp\"
        XENOS_RECOMP_INCLUDE_INPUT=\"${SKATE3_RECOMP_TOOLS}/XenosRecomp-Skate-3/XenosRecomp/shader_common.h\"
)

add_custom_command(
        OUTPUT
            "${CMAKE_CURRENT_SOURCE_DIR}/out/shaders/shader_cache.cpp"
        COMMAND
            $<TARGET_FILE:XenosRecomp>
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/skate-3/skate-3/data/big/shaders_final.big"
            ${XENOS_RECOMP_SOURCES}
            ${XENOS_RECOMP_INCLUDE}
        USES_TERMINAL
)

add_library(Skate3RecompLib
        ${SKATE3_RECOMP_PPC_DEFAULT_RECOMPILED_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/out/shaders/shader_cache.cpp"
)

# add include directories to here and simde in xenon recomp
target_include_directories(Skate3RecompLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Skate3RecompLib PRIVATE "${SKATE3_RECOMP_TOOLS}/XenonRecomp-Skate-3/thirdparty/simde")

# precompile ppc_recomp_shared header
#target_precompile_headers(Skate3RecompLib PUBLIC "out/ppc/ppc_recomp_shared.h")
