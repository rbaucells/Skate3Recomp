project("Skate3Recomp")

if (WIN32)
    option(SKATE3_RECOMP_D3D12 "Add D3D12 support for rendering" ON)
endif()

function(BIN2C)
    wocmake_parse_arguments(BIN2C_ARGS "" "TARGET_OBJ;SOURCE_FILE;DEST_FILE;ARRAY_NAME;COMPRESSION_TYPE" "" ${ARGN})

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

set(UNLEASHED_RECOMP_UI_CXX_SOURCES
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
)

set_source_files_properties(${SKATE3_RECOMP_THIRDPARTY_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

set(SKATE3_RECOMP_CXX_SOURCES
        "preload_executable.cpp"
        "sdl_listener.cpp"
        "stdafx.cpp"

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