#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "xex_patcher.h"
#include "directory_file_system.h"
#include "xcontent_file_system.h"

const std::filesystem::path skate_3_base = "/Users/ricardo/Projects/skate-3/game";
const std::filesystem::path skate_3_patch_tu3 = "/Users/ricardo/Projects/skate-3/TU_12K2276_000000C000000.00000000000O3";

const std::filesystem::path sonic_base = "/Users/ricardo/Projects/sonic/game";
const std::filesystem::path sonic_patch_tu2 = "/Users/ricardo/Projects/sonic/TU_19KA20I_000000C000000.00000000000G3";

const std::filesystem::path patched_out = "/Users/ricardo/Projects/Skate3Recomp/tools/xex_patcher/patched.xex";

constexpr std::string_view results[] = {"Success", "FileOpenFailed", "FileWriteFailed", "XexFileUnsupported", "XexFileInvalid", "PatchFileInvalid", "PatchIncompatible", "PatchFailed", "PatchUnsupported"};

int main() {
    // const std::unique_ptr<VirtualFileSystem> baseVfs = std::make_unique<DirectoryFileSystem>(skate_3_base);
    // const std::unique_ptr<VirtualFileSystem> patchVfs = std::make_unique<XContentFileSystem>(skate_3_patch_tu3);

    const std::unique_ptr<VirtualFileSystem> baseVfs = std::make_unique<DirectoryFileSystem>(sonic_base);
    const std::unique_ptr<VirtualFileSystem> patchVfs = std::make_unique<XContentFileSystem>(sonic_patch_tu2);

    std::vector<uint8_t> xexBytes;
    std::vector<uint8_t> patchBytes;
    if (!baseVfs->load("default.xex", xexBytes))
    {
        std::cout << "FileOpenFailed" << std::endl;;
        return (int)XexPatcher::Result::FileOpenFailed;
    }

    if (!patchVfs->load("default.xexp", patchBytes))
    {
        std::cout << "FileOpenFailed" << std::endl;;
        return (int)XexPatcher::Result::FileOpenFailed;
    }

    std::vector<uint8_t> patchedBytes;
    XexPatcher::Result result = XexPatcher::apply(xexBytes.data(), xexBytes.size(), patchBytes.data(), patchBytes.size(), patchedBytes, true);

    std::cout << results[(int)result] << std::endl;;

    std::cout << "Writing to file: " << patched_out << std::endl;

    FILE* fptr = fopen(patched_out.c_str(), "w");

    if (fptr == nullptr) {
        std::cout << "FileWriteFailed" << std::endl;
        return (int)XexPatcher::Result::FileWriteFailed;
    }

    fwrite(patchedBytes.data(), sizeof(uint8_t), patchedBytes.size(), fptr);

    fclose(fptr);

    return (int)result;
}