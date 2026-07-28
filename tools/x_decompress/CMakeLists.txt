project("x_decompress")
set(CMAKE_CXX_STANDARD 17)

add_executable(x_decompress
    "x_decompress.cpp"
    "${SKATE3_RECOMP_TOOLS}/XenonRecomp-Skate-3/thirdparty/libmspack/libmspack/mspack/lzxd.c"
)

target_include_directories(x_decompress
    PRIVATE "${SKATE3_RECOMP_TOOLS}/XenonRecomp-Skate-3/thirdparty/libmspack/libmspack/mspack"
)
