project("Skate3Recomp")

if (WIN32)
    option(SKATE3_RECOMP_D3D12 "Add D3D12 support for rendering" ON)
endif()

function(BIN2C)
    cmake_parse_arguments(BIN2C_ARGS "" "TARGET_OBJ;SOURCE_FILE;DEST_FILE;ARRAY_NAME;COMPRESSION_TYPE" "" ${ARGN})

    if(NOT BIN2C_ARGS_TARGET_OBJ)
        message(FATAL_ERROR "TARGET_OBJ not specified.")
    endif()

    if(NOT BIN2C_ARGS_SOURCE_FILE)
        message(FATAL_ERROR "SOURCE_FILE not specified.")
    endif()

    if(NOT BIN2C_ARGS_DEST_FILE)
        set(BIN2C_ARGS_DEST_FILE "${BIN2C_ARGS_SOURCE_FILE}")
    endif()

    if(NOT BIN2C_ARGS_COMPRESSION_TYPE)
        set(BIN2C_ARGS_COMPRESSION_TYPE "none")
    endif()

    add_custom_command(OUTPUT "${BIN2C_ARGS_DEST_FILE}.c"
            COMMAND $<TARGET_FILE:file_to_c> "${BIN2C_ARGS_SOURCE_FILE}" "${BIN2C_ARGS_ARRAY_NAME}" "${BIN2C_ARGS_COMPRESSION_TYPE}" "${BIN2C_ARGS_DEST_FILE}.c" "${BIN2C_ARGS_DEST_FILE}.h"
            DEPENDS "${BIN2C_ARGS_SOURCE_FILE}"
            BYPRODUCTS "${BIN2C_ARGS_DEST_FILE}.h"
            COMMENT "Generating binary header for ${BIN2C_ARGS_SOURCE_FILE}..."
    )

    set_source_files_properties(${BIN2C_ARGS_DEST_FILE}.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    target_sources(${BIN2C_ARGS_TARGET_OBJ} PRIVATE ${BIN2C_ARGS_DEST_FILE}.c)
endfunction()

add_compile_options(
        -fno-strict-aliasing
        -Wno-switch
        -Wno-unused-function
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -Wno-void-pointer-to-int-cast
        -Wno-int-to-void-pointer-cast
        -Wno-invalid-offsetof
        -Wno-null-arithmetic
        -Wno-null-conversion
        -Wno-tautological-undefined-compare
)

if (WIN32)
    add_compile_options(/fp:strict)
else()
    add_compile_options(-ffp-model=strict)
endif()

add_compile_definitions(SDL_MAIN_HANDLED _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR _CRT_SECURE_NO_WARNINGS)

set(SKATE3_RECOMP_PRECOMPILED_HEADERS "stdafx.h")

set(SKATE3_RECOMP_KERNEL_CXX_SOURCES
        "kernel/imports.cpp"
        "kernel/xdm.cpp"
        "kernel/heap.cpp"
        "kernel/memory.cpp"
        "kernel/xam.cpp"
        "kernel/io/file_system.cpp"
)

set(SKATE3_RECOMP_LOCALE_CXX_SOURCES
        "locale/config_locale.cpp"
        "locale/locale.cpp"
)

if (WIN32)
    set(SKATE3_RECOMP_OS_CXX_SOURCES
            "os/win32/logger_win32.cpp"
            "os/win32/media_win32.cpp"
            "os/win32/process_win32.cpp"
            "os/win32/user_win32.cpp"
            "os/win32/version_win32.cpp"
    )
elseif (APPLE)
    set(SKATE3_RECOMP_OS_CXX_SOURCES
            "os/macos/logger_macos.cpp"
            "os/macos/media_macos.cpp"
            "os/macos/process_macos.cpp"
            "os/macos/user_macos.cpp"
            "os/macos/version_macos.cpp"
    )
endif()

set(SKATE3_RECOMP_CPU_CXX_SOURCES
        "cpu/guest_thread.cpp"
)

set(SKATE3_RECOMP_GPU_CXX_SOURCES
        "gpu/video.cpp"
        "gpu/imgui/imgui_common.cpp"
        "gpu/imgui/imgui_font_builder.cpp"
        "gpu/imgui/imgui_snapshot.cpp"
)

set(SKATE3_RECOMP_APU_CXX_SOURCES
        "apu/audio.cpp"
        "apu/embedded_player.cpp"
        "apu/driver/sdl2_driver.cpp"
)

set(SKATE3_RECOMP_HID_CXX_SOURCES
        "hid/hid.cpp"
        "hid/driver/sdl_hid.cpp"
)

set(SKATE3_RECOMP_UI_CXX_SOURCES
        "ui/achievement_menu.cpp"
        "ui/achievement_overlay.cpp"
        "ui/black_bar.cpp"
        "ui/button_guide.cpp"
        "ui/fader.cpp"
        "ui/game_window.cpp"
        "ui/imgui_utils.cpp"
        "ui/installer_wizard.cpp"
        "ui/message_window.cpp"
        "ui/options_menu.cpp"
        "ui/options_menu_thumbnails.cpp"
        "ui/tv_static.cpp"
)

set(SKATE3_RECOMP_INSTALL_CXX_SOURCES
        "install/installer.cpp"
        "install/iso_file_system.cpp"
        "install/update_checker.cpp"
        "install/xcontent_file_system.cpp"
        "install/hashes/game.cpp"
        "install/hashes/update.cpp"
        "install/hashes/after_dark.cpp"
        "install/hashes/black_box.cpp"
        "install/hashes/danny_way.cpp"
        "install/hashes/maloof_money_cup.cpp"
        "install/hashes/san_van_party.cpp"
        "install/hashes/skate_share.cpp"
        "install/hashes/skate_create.cpp"
        "install/hashes/time_is_money.cpp"
)

set(SKATE3_RECOMP_USER_CXX_SOURCES
        "user/achievement_data.cpp"
        "user/achievement_manager.cpp"
        "user/config.cpp"
        "user/registry.cpp"
        "user/paths.cpp"
        "user/persistent_data.cpp"
        "user/persistent_storage_manager.cpp"
)

set(SKATE3_RECOMP_THIRDPARTY_SOURCES
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/backends/imgui_impl_sdl2.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/imgui.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/imgui_demo.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/imgui_draw.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/imgui_tables.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui/imgui_widgets.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/implot/implot.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/implot/implot_demo.cpp"
        "${SKATE3_RECOMP_THIRDPARTY}/implot/implot_items.cpp"
        "${SKATE3_RECOMP_TOOLS}/XenosRecomp-Skate-3/thirdparty/smol-v/source/smolv.cpp"
)

set(SKATE3_RECOMP_THIRDPARTY_INCLUDES
        "${SKATE3_RECOMP_THIRDPARTY}/concurrentqueue"
        "${SKATE3_RECOMP_THIRDPARTY}/ddspp"
        "${SKATE3_RECOMP_THIRDPARTY}/imgui"
        "${SKATE3_RECOMP_THIRDPARTY}/implot"
        "${SKATE3_RECOMP_THIRDPARTY}/json/include"
        "${SKATE3_RECOMP_THIRDPARTY}/magic_enum/include"
        "${SKATE3_RECOMP_THIRDPARTY}/stb"
        "${SKATE3_RECOMP_THIRDPARTY}/unordered_dense/include"
        "${SKATE3_RECOMP_TOOLS}/bc_diff"
        "${SKATE3_RECOMP_TOOLS}/XenosRecomp-Skate-3/thirdparty/smol-v/source"
        "${SKATE3_RECOMP_THIRDPARTY}/include"
)

set_source_files_properties(${SKATE3_RECOMP_THIRDPARTY_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

set(SKATE3_RECOMP_CXX_SOURCES
        "preload_executable.cpp"
        "sdl_listener.cpp"
        "stdafx.cpp"
        "app.cpp"

        ${SKATE3_RECOMP_KERNEL_CXX_SOURCES}
        ${SKATE3_RECOMP_LOCALE_CXX_SOURCES}
        ${SKATE3_RECOMP_OS_CXX_SOURCES}
        ${SKATE3_RECOMP_CPU_CXX_SOURCES}
        ${SKATE3_RECOMP_GPU_CXX_SOURCES}
        ${SKATE3_RECOMP_APU_CXX_SOURCES}
        ${SKATE3_RECOMP_HID_CXX_SOURCES}
        ${SKATE3_RECOMP_PATCHES_CXX_SOURCES}
        ${SKATE3_RECOMP_UI_CXX_SOURCES}
        ${SKATE3_RECOMP_INSTALL_CXX_SOURCES}
        ${SKATE3_RECOMP_USER_CXX_SOURCES}
        ${SKATE3_RECOMP_MOD_CXX_SOURCES}
        ${SKATE3_RECOMP_THIRDPARTY_SOURCES}
)

add_executable(Skate3Recomp ${SKATE3_RECOMP_CXX_SOURCES})

find_package(CURL REQUIRED)

if (SKATE3_RECOMP_D3D12)
    find_package(directx-headers CONFIG REQUIRED)
    find_package(directx12-agility CONFIG REQUIRED)
    target_compile_definitions(Skate3Recomp PRIVATE SKATE3_RECOMP_D3D12)

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/D3D12)
    add_custom_command(TARGET Skate3Recomp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Core,IMPORTED_LOCATION_RELEASE> $<TARGET_FILE_DIR:Skate3Recomp>/D3D12
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Layers,IMPORTED_LOCATION_DEBUG> $<TARGET_FILE_DIR:Skate3Recomp>/D3D12
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectXShaderCompiler,IMPORTED_LOCATION> $<TARGET_FILE_DIR:Skate3Recomp>
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DXIL,IMPORTED_LOCATION> $<TARGET_FILE_DIR:Skate3Recomp>
            COMMAND_EXPAND_LISTS
    )

    target_link_libraries(Skate3Recomp PRIVATE
            Microsoft::DirectXShaderCompiler
            Microsoft::DXIL
            dxgi
    )
endif()

if (WIN32)
    target_link_libraries(Skate3Recomp PRIVATE
            comctl32
            dwmapi
            ntdll
            Shcore
            Synchronization
            winmm
            windowsapp
    )
endif()

target_link_libraries(Skate3Recomp PRIVATE
        fmt::fmt
        libzstd_static
        msdf-atlas-gen::msdf-atlas-gen
        nfd::nfd
        o1heap
        XenonUtils
        SDL2::SDL2-static
        SDL2_mixer
        tomlplusplus::tomlplusplus
        Skate3RecompLib
        xxHash::xxhash
        CURL::libcurl
        plume
)

target_include_directories(Skate3Recomp PRIVATE "${SKATE3_RECOMP_LIB}/out" ${CMAKE_CURRENT_SOURCE_DIR} ${SKATE3_RECOMP_THIRDPARTY_INCLUDES})

# precompile headers
#target_precompile_headers(Skate3Recomp PUBLIC ${SKATE3_RECOMP_PRECOMPILED_HEADERS})

function(compile_shader FILE_PATH TARGET_NAME)
    set(FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/gpu/shader/${FILE_PATH}.hlsl)
    cmake_path(GET FILE_PATH STEM VARIABLE_NAME)
    if (SKATE3_RECOMP_D3D12)
        add_custom_command(
                OUTPUT ${FILE_PATH}.dxil.h
                COMMAND ${DIRECTX_DXC_TOOL} -T ${TARGET_NAME} -HV 2021 -all-resources-bound -Wno-ignored-attributes -Fh ${FILE_PATH}.dxil.h ${FILE_PATH} -Vn g_${VARIABLE_NAME}_dxil
                DEPENDS ${FILE_PATH}
        )
        target_sources(Skate3Recomp PRIVATE ${FILE_PATH}.dxil.h)
    endif()
    add_custom_command(
            OUTPUT ${FILE_PATH}.spirv.h
            COMMAND ${DIRECTX_DXC_TOOL} -T ${TARGET_NAME} -HV 2021 -all-resources-bound -spirv -fvk-use-dx-layout ${ARGN} -Fh ${FILE_PATH}.spirv.h ${FILE_PATH} -Vn g_${VARIABLE_NAME}_spirv
            DEPENDS ${FILE_PATH}
    )
    target_sources(Skate3Recomp PRIVATE ${FILE_PATH}.spirv.h)
endfunction()

function(compile_vertex_shader FILE_PATH)
    compile_shader(${FILE_PATH} vs_6_0 -fvk-invert-y)
endfunction()

function(compile_pixel_shader FILE_PATH)
    compile_shader(${FILE_PATH} ps_6_0)
endfunction()

compile_pixel_shader(blend_color_alpha_ps)
compile_vertex_shader(copy_vs)
compile_pixel_shader(copy_color_ps)
compile_pixel_shader(copy_depth_ps)
compile_pixel_shader(csd_filter_ps)
compile_vertex_shader(csd_no_tex_vs)
compile_vertex_shader(csd_vs)
compile_pixel_shader(enhanced_motion_blur_ps)
compile_pixel_shader(gaussian_blur_3x3)
compile_pixel_shader(gaussian_blur_5x5)
compile_pixel_shader(gaussian_blur_7x7)
compile_pixel_shader(gaussian_blur_9x9)
compile_pixel_shader(gamma_correction_ps)
compile_pixel_shader(imgui_ps)
compile_vertex_shader(imgui_vs)
compile_pixel_shader(movie_ps)
compile_vertex_shader(movie_vs)
compile_pixel_shader(resolve_msaa_color_2x)
compile_pixel_shader(resolve_msaa_color_4x)
compile_pixel_shader(resolve_msaa_color_8x)
compile_pixel_shader(resolve_msaa_depth_2x)
compile_pixel_shader(resolve_msaa_depth_4x)
compile_pixel_shader(resolve_msaa_depth_8x)

set(RESOURCES_SOURCE_PATH "${PROJECT_SOURCE_DIR}/../Skate3RecompResources")
set(RESOURCES_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/res")

## Miscellaneous ##
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/bc_diff/button_bc_diff.bin" DEST_FILE "${RESOURCES_OUTPUT_PATH}/bc_diff/button_bc_diff.bin" ARRAY_NAME "g_button_bc_diff" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/font/im_font_atlas.bin" DEST_FILE "${RESOURCES_OUTPUT_PATH}/font/im_font_atlas.bin" ARRAY_NAME "g_im_font_atlas" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/font/im_font_atlas.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/font/im_font_atlas.dds" ARRAY_NAME "g_im_font_atlas_texture" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/achievements_menu/trophy.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/achievements_menu/trophy.dds" ARRAY_NAME "g_trophy" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/controller.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/controller.dds" ARRAY_NAME "g_controller" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/general_window.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/general_window.dds" ARRAY_NAME "g_general_window" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/hedge-dev.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/hedge-dev.dds" ARRAY_NAME "g_hedgedev" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/kbm.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/kbm.dds" ARRAY_NAME "g_kbm" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/select.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/select.dds" ARRAY_NAME "g_select" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/common/light.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/common/light.dds" ARRAY_NAME "g_light" COMPRESSION_TYPE "zstd")

## Installer ##
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/arrow_circle.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/arrow_circle.dds" ARRAY_NAME "g_arrow_circle" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_001.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_001.dds" ARRAY_NAME "g_install_001" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_002.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_002.dds" ARRAY_NAME "g_install_002" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_003.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_003.dds" ARRAY_NAME "g_install_003" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_004.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_004.dds" ARRAY_NAME "g_install_004" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_005.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_005.dds" ARRAY_NAME "g_install_005" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_006.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_006.dds" ARRAY_NAME "g_install_006" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_007.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_007.dds" ARRAY_NAME "g_install_007" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/install_008.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/install_008.dds" ARRAY_NAME "g_install_008" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/miles_electric_icon.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/miles_electric_icon.dds" ARRAY_NAME "g_miles_electric_icon" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/installer/pulse_install.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/installer/pulse_install.dds" ARRAY_NAME "g_pulse_install" COMPRESSION_TYPE "zstd")

## Options Menu ##
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/achievement_notifications.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/achievement_notifications.dds" ARRAY_NAME "g_achievement_notifications" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/allow_background_input_ps.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/allow_background_input_ps.dds" ARRAY_NAME "g_allow_background_input_ps" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/allow_background_input_xb.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/allow_background_input_xb.dds" ARRAY_NAME "g_allow_background_input_xb" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/antialiasing_none.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/antialiasing_none.dds" ARRAY_NAME "g_antialiasing_none" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/antialiasing_2x.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/antialiasing_2x.dds" ARRAY_NAME "g_antialiasing_2x" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/antialiasing_4x.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/antialiasing_4x.dds" ARRAY_NAME "g_antialiasing_4x" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/antialiasing_8x.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/antialiasing_8x.dds" ARRAY_NAME "g_antialiasing_8x" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/aspect_ratio.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/aspect_ratio.dds" ARRAY_NAME "g_aspect_ratio" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/battle_theme.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/battle_theme.dds" ARRAY_NAME "g_battle_theme" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/brightness.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/brightness.dds" ARRAY_NAME "g_brightness" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/channel_stereo.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/channel_stereo.dds" ARRAY_NAME "g_channel_stereo" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/channel_surround.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/channel_surround.dds" ARRAY_NAME "g_channel_surround" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/control_tutorial_ps.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/control_tutorial_ps.dds" ARRAY_NAME "g_control_tutorial_ps" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/control_tutorial_xb.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/control_tutorial_xb.dds" ARRAY_NAME "g_control_tutorial_xb" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/controller_icons.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/controller_icons.dds" ARRAY_NAME "g_controller_icons" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/default.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/default.dds" ARRAY_NAME "g_default" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/effects_volume.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/effects_volume.dds" ARRAY_NAME "g_effects_volume" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/fps.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/fps.dds" ARRAY_NAME "g_fps" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/fullscreen.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/fullscreen.dds" ARRAY_NAME "g_fullscreen" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/gi_texture_filtering_bilinear.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/gi_texture_filtering_bilinear.dds" ARRAY_NAME "g_gi_texture_filtering_bilinear" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/gi_texture_filtering_bicubic.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/gi_texture_filtering_bicubic.dds" ARRAY_NAME "g_gi_texture_filtering_bicubic" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/hints.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/hints.dds" ARRAY_NAME "g_hints" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/horizontal_camera.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/horizontal_camera.dds" ARRAY_NAME "g_horizontal_camera" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/language.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/language.dds" ARRAY_NAME "g_language" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/master_volume.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/master_volume.dds" ARRAY_NAME "g_master_volume" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/monitor.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/monitor.dds" ARRAY_NAME "g_monitor" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/motion_blur_off.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/motion_blur_off.dds" ARRAY_NAME "g_motion_blur_off" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/motion_blur_original.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/motion_blur_original.dds" ARRAY_NAME "g_motion_blur_original" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/motion_blur_enhanced.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/motion_blur_enhanced.dds" ARRAY_NAME "g_motion_blur_enhanced" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/movie_scale_fit.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/movie_scale_fit.dds" ARRAY_NAME "g_movie_scale_fit" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/movie_scale_fill.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/movie_scale_fill.dds" ARRAY_NAME "g_movie_scale_fill" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/music_attenuation.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/music_attenuation.dds" ARRAY_NAME "g_music_attenuation" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/music_volume.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/music_volume.dds" ARRAY_NAME "g_music_volume" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/shadow_resolution_x512.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/shadow_resolution_x512.dds" ARRAY_NAME "g_shadow_resolution_x512" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/shadow_resolution_x1024.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/shadow_resolution_x1024.dds" ARRAY_NAME "g_shadow_resolution_x1024" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/shadow_resolution_x2048.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/shadow_resolution_x2048.dds" ARRAY_NAME "g_shadow_resolution_x2048" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/shadow_resolution_x4096.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/shadow_resolution_x4096.dds" ARRAY_NAME "g_shadow_resolution_x4096" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/shadow_resolution_x8192.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/shadow_resolution_x8192.dds" ARRAY_NAME "g_shadow_resolution_x8192" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/time_transition_ps.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/time_transition_ps.dds" ARRAY_NAME "g_time_of_day_transition_playstation" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/time_transition_xb.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/time_transition_xb.dds" ARRAY_NAME "g_time_of_day_transition_xbox" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/transparency_antialiasing_false.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/transparency_antialiasing_false.dds" ARRAY_NAME "g_transparency_antialiasing_false" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/transparency_antialiasing_true.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/transparency_antialiasing_true.dds" ARRAY_NAME "g_transparency_antialiasing_true" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/ui_alignment_centre.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/ui_alignment_centre.dds" ARRAY_NAME "g_ui_alignment_centre" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/ui_alignment_edge.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/ui_alignment_edge.dds" ARRAY_NAME "g_ui_alignment_edge" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/vertical_camera.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/vertical_camera.dds" ARRAY_NAME "g_vertical_camera" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/voice_language.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/voice_language.dds" ARRAY_NAME "g_voice_language" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/vibration_ps.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/vibration_ps.dds" ARRAY_NAME "g_vibration_ps" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/vibration_xb.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/vibration_xb.dds" ARRAY_NAME "g_vibration_xb" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/vsync_on.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/vsync_on.dds" ARRAY_NAME "g_vsync_on" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/vsync_off.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/vsync_off.dds" ARRAY_NAME "g_vsync_off" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/window_size.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/window_size.dds" ARRAY_NAME "g_window_size" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/thumbnails/xbox_color_correction.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/thumbnails/xbox_color_correction.dds" ARRAY_NAME "g_xbox_color_correction" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/miles_electric.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/miles_electric.dds" ARRAY_NAME "g_miles_electric" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/options_static.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/options_static.dds" ARRAY_NAME "g_options_static" COMPRESSION_TYPE "zstd")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/options_menu/options_static_flash.dds" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/options_menu/options_static_flash.dds" ARRAY_NAME "g_options_static_flash" COMPRESSION_TYPE "zstd")

## Game Icon ##
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/game_icon.bmp" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/game_icon.bmp" ARRAY_NAME "g_game_icon")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/images/game_icon_night.bmp" DEST_FILE "${RESOURCES_OUTPUT_PATH}/images/game_icon_night.bmp" ARRAY_NAME "g_game_icon_night")

## Audio ##
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/music/installer.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/music/installer.ogg" ARRAY_NAME "g_installer_music")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_worldmap_cursor.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_worldmap_cursor.ogg" ARRAY_NAME "g_sys_worldmap_cursor")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_worldmap_finaldecide.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_worldmap_finaldecide.ogg" ARRAY_NAME "g_sys_worldmap_finaldecide")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_actstg_pausecansel.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_actstg_pausecansel.ogg" ARRAY_NAME "g_sys_actstg_pausecansel")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_actstg_pausecursor.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_actstg_pausecursor.ogg" ARRAY_NAME "g_sys_actstg_pausecursor")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_actstg_pausedecide.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_actstg_pausedecide.ogg" ARRAY_NAME "g_sys_actstg_pausedecide")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_actstg_pausewinclose.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_actstg_pausewinclose.ogg" ARRAY_NAME "g_sys_actstg_pausewinclose")
BIN2C(TARGET_OBJ Skate3Recomp SOURCE_FILE "${RESOURCES_SOURCE_PATH}/sounds/sys_actstg_pausewinopen.ogg" DEST_FILE "${RESOURCES_OUTPUT_PATH}/sounds/sys_actstg_pausewinopen.ogg" ARRAY_NAME "g_sys_actstg_pausewinopen")
