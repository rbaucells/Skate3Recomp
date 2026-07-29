#include "installer.h"

#include <thread>
#include <xxh3.h>

#include "directory_file_system.h"
#include "iso_file_system.h"
#include "xcontent_file_system.h"

#include "hashes/game.h"
#include "hashes/update.h"
#include "hashes/after_dark.h"
#include "hashes/black_box.h"
#include "hashes/danny_way.h"
#include "hashes/maloof_money_cup.h"
#include "hashes/san_van_party.h"
#include "hashes/skate_share.h"
#include "hashes/skate_create.h"
#include "hashes/time_is_money.h"

#include "fmt/core.h"

static const std::string GameDirectory = "game";
static const std::string DLCDirectory = "dlc";
static const std::string PatchedDirectory = "patched";
static const std::string AfterDarkDirectory = DLCDirectory + "/After Dark";
static const std::string BlackBoxSkateParkDirectory = DLCDirectory + "/Black Box Skate Park";
static const std::string HawaiianDreamDirectory = DLCDirectory + "/Hawaiian Dream";
static const std::string MaloofMoneyCupDirectory = DLCDirectory + "/Maloof Money Cup";
static const std::string SanVanPartyPackDirectory = DLCDirectory + "/San Van Party Pack";
static const std::string SkateSharePackDirectory = DLCDirectory + "/Skate Share Pack";
static const std::string SkateCreateUpgradePackDirectory = DLCDirectory + "/Skate.Create Upgrade Pack";
static const std::string TimeIsMoneyPackDirectory = DLCDirectory + "/Time is Money Pack";
static const std::string UpdateDirectory = "update";
static const std::string GameExecutableFile = "default.xex";
static const std::string UpdateExecutablePatchFile = "default.xexp";
static const std::string ISOExtension = ".iso";
static const std::string OldExtension = ".old";
static const std::string TempExtension = ".tmp";

static std::string fromU8(const std::u8string &str)
{
    return std::string(str.begin(), str.end());
}

static std::string fromPath(const std::filesystem::path &path)
{
    return fromU8(path.u8string());
}

static std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
};

static std::string getDLCValidationFile(DLC dlc)
{
    switch (dlc)
    {
    case DLC::AfterDark:
        return "player1p_00000000.big";
    case DLC::BlackBoxSkatePark:
        return "presell_00000000.big";
    case DLC::HawaiianDream:
        return "dway_park_00000000.big";
    case DLC::MaloofMoneyCup:
        return "maloof_money_cup_00000000.big";
    case DLC::SanVanPartyPack:
        return "play_00000000.big";
    case DLC::SkateSharePack:
        return "project10_00000000.big";
    case DLC::SkateCreateUpgradePack:
        return "creator_00000000.big";
    case DLC::TimeIsMoneyPack:
        return "unlockall_00000000.big";
    default:
        return "";
    }
}

static DLC mapBigFileToDLC(const std::string &bigFile)
{
    if (bigFile == "player1p_00000000.big")
    {
        return DLC::AfterDark;
    }
    if (bigFile == "presell_00000000.big")
    {
        return DLC::BlackBoxSkatePark;
    }
    if (bigFile == "dway_park_00000000.big")
    {
        return DLC::HawaiianDream;
    }
    if (bigFile == "maloof_money_cup_00000000.big")
    {
        return DLC::MaloofMoneyCup;
    }
    if (bigFile == "play_00000000.big")
    {
        return DLC::SanVanPartyPack;
    }
    if (bigFile == "project10_00000000.big")
    {
        return DLC::SkateSharePack;
    }
    if (bigFile == "creator_00000000.big")
    {
        return DLC::SkateCreateUpgradePack;
    }
    if (bigFile == "unlockall_00000000.big")
    {
        return DLC::TimeIsMoneyPack;
    }

    return DLC::Unknown;
}

static DLC detectDLCFromXContent(XContentFileSystem *xcontentVfs)
{
    if (xcontentVfs == nullptr)
    {
        return DLC::Unknown;
    }

    for (const auto &file : xcontentVfs->fileMap)
    {
        const std::string &fileName = file.first;
        size_t dotPos = fileName.rfind('.');
        if (dotPos != std::string::npos && fileName.substr(dotPos) == ".big")
        {
            return mapBigFileToDLC(fileName);
        }
    }

    return DLC::Unknown;
}

static std::unique_ptr<VirtualFileSystem> createFileSystemFromPath(const std::filesystem::path &path)
{
    if (XContentFileSystem::check(path))
    {
        return XContentFileSystem::create(path);
    }
    else if (toLower(fromPath(path.extension())) == ISOExtension)
    {
        return ISOFileSystem::create(path);
    }
    else if (std::filesystem::is_directory(path))
    {
        return DirectoryFileSystem::create(path);
    }
    else
    {
        return nullptr;
    }
}

static bool checkFile(const FilePair &pair, const uint64_t *fileHashes, const std::filesystem::path &targetDirectory, std::vector<uint8_t> &fileData, Journal &journal, const std::function<bool()> &progressCallback, bool checkSizeOnly) {
    const std::string fileName(pair.first);
    const uint32_t hashCount = pair.second;
    const std::filesystem::path filePath = targetDirectory / fileName;
    if (!std::filesystem::exists(filePath))
    {
        journal.lastResult = Journal::Result::FileMissing;
        journal.lastErrorMessage = fmt::format("File {} does not exist.", fileName);
        return false;
    }

    std::error_code ec;
    size_t fileSize = std::filesystem::file_size(filePath, ec);
    if (ec)
    {
        journal.lastResult = Journal::Result::FileReadFailed;
        journal.lastErrorMessage = fmt::format("Failed to read file size for {}.", fileName);
        return false;
    }

    if (checkSizeOnly)
    {
        journal.progressTotal += fileSize;
    }
    else
    {
        std::ifstream fileStream(filePath, std::ios::binary);
        if (fileStream.is_open())
        {
            fileData.resize(fileSize);
            fileStream.read((char *)(fileData.data()), fileSize);
        }

        if (!fileStream.is_open() || fileStream.bad())
        {
            journal.lastResult = Journal::Result::FileReadFailed;
            journal.lastErrorMessage = fmt::format("Failed to read file {}.", fileName);
            return false;
        }

        uint64_t fileHash = XXH3_64bits(fileData.data(), fileSize);
        bool fileHashFound = false;
        for (uint32_t i = 0; i < hashCount && !fileHashFound; i++)
        {
            fileHashFound = fileHash == fileHashes[i];
        }

        if (!fileHashFound)
        {
            journal.lastResult = Journal::Result::FileHashFailed;
            journal.lastErrorMessage = fmt::format("File {} did not match any of the known hashes.", fileName);
            return false;
        }

        journal.progressCounter += fileSize;
    }

    if (!progressCallback())
    {
        journal.lastResult = Journal::Result::Cancelled;
        journal.lastErrorMessage = "Check was cancelled.";
        return false;
    }

    return true;
}

static bool copyFile(const FilePair &pair, const uint64_t *fileHashes, VirtualFileSystem &sourceVfs, const std::filesystem::path &targetDirectory, bool skipHashChecks, std::vector<uint8_t> &fileData, Journal &journal, const std::function<bool()> &progressCallback) {
    const std::string filename(pair.first);
    const uint32_t hashCount = pair.second;
    if (!sourceVfs.exists(filename))
    {
        journal.lastResult = Journal::Result::FileMissing;
        journal.lastErrorMessage = fmt::format("File {} does not exist in {}.", filename, sourceVfs.getName());
        return false;
    }

    if (!sourceVfs.load(filename, fileData))
    {
        journal.lastResult = Journal::Result::FileReadFailed;
        journal.lastErrorMessage = fmt::format("Failed to read file {} from {}.", filename, sourceVfs.getName());
        return false;
    }

    if (!skipHashChecks)
    {
        uint64_t fileHash = XXH3_64bits(fileData.data(), fileData.size());
        bool fileHashFound = false;
        for (uint32_t i = 0; i < hashCount && !fileHashFound; i++)
        {
            fileHashFound = fileHash == fileHashes[i];
        }

        if (!fileHashFound)
        {
            journal.lastResult = Journal::Result::FileHashFailed;
            journal.lastErrorMessage = fmt::format("File {} from {} did not match any of the known hashes.", filename, sourceVfs.getName());
            return false;
        }
    }

    std::filesystem::path targetPath = targetDirectory / std::filesystem::path(std::u8string_view((const char8_t *)(pair.first)));
    std::filesystem::path parentPath = targetPath.parent_path();
    if (!std::filesystem::exists(parentPath))
    {
        std::error_code ec;
        std::filesystem::create_directories(parentPath, ec);
    }
    
    while (!parentPath.empty()) {
        journal.createdDirectories.insert(parentPath);

        if (parentPath != targetDirectory) {
            parentPath = parentPath.parent_path();
        }
        else {
            parentPath = std::filesystem::path();
        }
    }

    std::ofstream outStream(targetPath, std::ios::binary);
    if (!outStream.is_open())
    {
        journal.lastResult = Journal::Result::FileCreationFailed;
        journal.lastErrorMessage = fmt::format("Failed to create file at {}.", fromPath(targetPath));
        return false;
    }

    journal.createdFiles.push_back(targetPath);

    outStream.write((const char *)(fileData.data()), fileData.size());
    if (outStream.bad())
    {
        journal.lastResult = Journal::Result::FileWriteFailed;
        journal.lastErrorMessage = fmt::format("Failed to create file at {}.", fromPath(targetPath));
        return false;
    }

    journal.progressCounter += fileData.size();
    
    if (!progressCallback())
    {
        journal.lastResult = Journal::Result::Cancelled;
        journal.lastErrorMessage = "Installation was cancelled.";
        return false;
    }

    return true;
}

static DLC detectDLC(const std::filesystem::path &sourcePath, VirtualFileSystem &sourceVfs, Journal &journal)
{
    if (auto *xcontentVfs = dynamic_cast<XContentFileSystem *>(&sourceVfs))
    {
        DLC dlc = detectDLCFromXContent(xcontentVfs);
        if (dlc != DLC::Unknown)
        {
            return dlc;
        }
    }

    journal.lastResult = Journal::Result::UnknownDLCType;
    journal.lastErrorMessage = fmt::format("DLC type for {} is unknown.", sourceVfs.getName());
    return DLC::Unknown;
}

static bool fillDLCSource(DLC dlc, Installer::DLCSource &dlcSource) 
{
    switch (dlc)
    {
    case DLC::AfterDark:
        dlcSource.filePairs = { Dlc1Files, Dlc1FilesSize };
        dlcSource.fileHashes = Dlc1Hashes;
        dlcSource.targetSubDirectory = AfterDarkDirectory;
        dlcSource.validationFile = "player1p_00000000.big";
        return true;
    case DLC::BlackBoxSkatePark:
        dlcSource.filePairs = { Dlc2Files, Dlc2FilesSize };
        dlcSource.fileHashes = Dlc2Hashes;
        dlcSource.targetSubDirectory = BlackBoxSkateParkDirectory;
        dlcSource.validationFile = "presell_00000000.big";
        return true;
    case DLC::HawaiianDream:
        dlcSource.filePairs = { Dlc3Files, Dlc3FilesSize };
        dlcSource.fileHashes = Dlc3Hashes;
        dlcSource.targetSubDirectory = HawaiianDreamDirectory;
        dlcSource.validationFile = "dway_park_00000000.big";
        return true;
    case DLC::MaloofMoneyCup:
        dlcSource.filePairs = { Dlc4Files, Dlc4FilesSize };
        dlcSource.fileHashes = Dlc4Hashes;
        dlcSource.targetSubDirectory = MaloofMoneyCupDirectory;
        dlcSource.validationFile = "maloof_money_cup_00000000.big";
        return true;
    case DLC::SanVanPartyPack:
        dlcSource.filePairs = { Dlc5Files, Dlc5FilesSize };
        dlcSource.fileHashes = Dlc5Hashes;
        dlcSource.targetSubDirectory = SanVanPartyPackDirectory;
        dlcSource.validationFile = "play_00000000.big";
        return true;
    case DLC::SkateSharePack:
        dlcSource.filePairs = { Dlc6Files, Dlc6FilesSize };
        dlcSource.fileHashes = Dlc6Hashes;
        dlcSource.targetSubDirectory = SkateSharePackDirectory;
        dlcSource.validationFile = "project10_00000000.big";
        return true;
    case DLC::SkateCreateUpgradePack:
        dlcSource.filePairs = { Dlc7Files, Dlc7FilesSize };
        dlcSource.fileHashes = Dlc7Hashes;
        dlcSource.targetSubDirectory = SkateCreateUpgradePackDirectory;
        dlcSource.validationFile = "creator_00000000.big";
        return true;
    case DLC::TimeIsMoneyPack:
        dlcSource.filePairs = { Dlc8Files, Dlc8FilesSize };
        dlcSource.fileHashes = Dlc8Hashes;
        dlcSource.targetSubDirectory = TimeIsMoneyPackDirectory;
        dlcSource.validationFile = "unlockall_00000000.big";
        return true;
    default:
        return false;
    }
}

bool Installer::checkGameInstall(const std::filesystem::path &baseDirectory, std::filesystem::path &modulePath)
{
    modulePath = baseDirectory / PatchedDirectory / GameExecutableFile;

    if (!std::filesystem::exists(modulePath))
        return false;

    if (!std::filesystem::exists(baseDirectory / UpdateDirectory / UpdateExecutablePatchFile))
        return false;

    if (!std::filesystem::exists(baseDirectory / GameDirectory / GameExecutableFile))
        return false;

    return true;
}

bool Installer::checkDLCInstall(const std::filesystem::path &baseDirectory, DLC dlc)
{
    std::string validationFile = getDLCValidationFile(dlc);
    if (validationFile.empty())
    {
        return false;
    }

    switch (dlc)
    {
    case DLC::AfterDark:
        return std::filesystem::exists(baseDirectory / AfterDarkDirectory / validationFile);
    case DLC::BlackBoxSkatePark:
        return std::filesystem::exists(baseDirectory / BlackBoxSkateParkDirectory / validationFile);
    case DLC::HawaiianDream:
        return std::filesystem::exists(baseDirectory / HawaiianDreamDirectory / validationFile);
    case DLC::MaloofMoneyCup:
        return std::filesystem::exists(baseDirectory / MaloofMoneyCupDirectory / validationFile);
    case DLC::SanVanPartyPack:
        return std::filesystem::exists(baseDirectory / SanVanPartyPackDirectory / validationFile);
    case DLC::SkateSharePack:
        return std::filesystem::exists(baseDirectory / SkateSharePackDirectory / validationFile);
    case DLC::SkateCreateUpgradePack:
        return std::filesystem::exists(baseDirectory / SkateCreateUpgradePackDirectory / validationFile);
    case DLC::TimeIsMoneyPack:
        return std::filesystem::exists(baseDirectory / TimeIsMoneyPackDirectory / validationFile);
    default:
        return false;
    }
}

bool Installer::checkAllDLC(const std::filesystem::path& baseDirectory)
{
    bool result = true;

    for (int i = 1; i < (int)DLC::Count; i++)
    {
        if (!checkDLCInstall(baseDirectory, (DLC)i))
            result = false;
    }

    return result;
}

bool Installer::checkInstallIntegrity(const std::filesystem::path &baseDirectory, Journal &journal, const std::function<bool()> &progressCallback)
{
    // Run the file checks twice: once to fill out the progress counter and the file sizes, and another pass to do the hash integrity checks.
    for (uint32_t checkPass = 0; checkPass < 2; checkPass++)
    {
        bool checkSizeOnly = (checkPass == 0);
        if (!checkFiles({ GameFiles, GameFilesSize }, GameHashes, baseDirectory / GameDirectory, journal, progressCallback, checkSizeOnly))
        {
            return false;
        }

        if (!checkFiles({ UpdateFiles, UpdateFilesSize }, UpdateHashes, baseDirectory / UpdateDirectory, journal, progressCallback, checkSizeOnly))
        {
            return false;
        }

        for (int i = 1; i < (int)DLC::Count; i++)
        {
            if (checkDLCInstall(baseDirectory, (DLC)i))
            {
                Installer::DLCSource dlcSource;
                fillDLCSource((DLC)i, dlcSource);

                if (!checkFiles(dlcSource.filePairs, dlcSource.fileHashes, baseDirectory / dlcSource.targetSubDirectory, journal, progressCallback, checkSizeOnly))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool Installer::computeTotalSize(std::span<const FilePair> filePairs, const uint64_t *fileHashes, VirtualFileSystem &sourceVfs, Journal &journal, uint64_t &totalSize)
{
    for (FilePair pair : filePairs)
    {
        const std::string filename(pair.first);
        if (!sourceVfs.exists(filename))
        {
            journal.lastResult = Journal::Result::FileMissing;
            journal.lastErrorMessage = fmt::format("File {} does not exist in {}.", filename, sourceVfs.getName());
            return false;
        }

        totalSize += sourceVfs.getSize(filename);
    }

    return true;
}

bool Installer::checkFiles(std::span<const FilePair> filePairs, const uint64_t *fileHashes, const std::filesystem::path &targetDirectory, Journal &journal, const std::function<bool()> &progressCallback, bool checkSizeOnly)
{
    FilePair validationPair = {};
    uint32_t validationHashIndex = 0;
    uint32_t hashIndex = 0;
    uint32_t hashCount = 0;
    std::vector<uint8_t> fileData;
    for (FilePair pair : filePairs)
    {
        hashIndex = hashCount;
        hashCount += pair.second;

        if (!checkFile(pair, &fileHashes[hashIndex], targetDirectory, fileData, journal, progressCallback, checkSizeOnly))
        {
            return false;
        }
    }

    return true;
}

bool Installer::copyFiles(std::span<const FilePair> filePairs, const uint64_t *fileHashes, VirtualFileSystem &sourceVfs, const std::filesystem::path &targetDirectory, const std::string &validationFile, bool skipHashChecks, Journal &journal, const std::function<bool()> &progressCallback)
{
    std::error_code ec;
    if (!std::filesystem::exists(targetDirectory) && !std::filesystem::create_directories(targetDirectory, ec))
    {
        journal.lastResult = Journal::Result::DirectoryCreationFailed;
        journal.lastErrorMessage = "Unable to create directory at " + fromPath(targetDirectory);
        return false;
    }

    FilePair validationPair = {};
    uint32_t validationHashIndex = 0;
    uint32_t hashIndex = 0;
    uint32_t hashCount = 0;
    std::vector<uint8_t> fileData;
    for (FilePair pair : filePairs)
    {
        hashIndex = hashCount;
        hashCount += pair.second;

        if (validationFile.compare(pair.first) == 0)
        {
            validationPair = pair;
            validationHashIndex = hashIndex;
            continue;
        }

        if (!copyFile(pair, &fileHashes[hashIndex], sourceVfs, targetDirectory, skipHashChecks, fileData, journal, progressCallback))
        {
            return false;
        }
    }

    // Validation file is copied last after all other files have been copied.
    if (validationPair.first != nullptr)
    {
        if (!copyFile(validationPair, &fileHashes[validationHashIndex], sourceVfs, targetDirectory, skipHashChecks, fileData, journal, progressCallback))
        {
            return false;
        }
    }
    else
    {
        journal.lastResult = Journal::Result::ValidationFileMissing;
        journal.lastErrorMessage = fmt::format("Unable to find validation file {} in {}.", validationFile, sourceVfs.getName());
        return false;
    }

    return true;
}

bool Installer::parseContent(const std::filesystem::path &sourcePath, std::unique_ptr<VirtualFileSystem> &targetVfs, Journal &journal)
{
    targetVfs = createFileSystemFromPath(sourcePath);
    if (targetVfs != nullptr)
    {
        return true;
    }
    else
    {
        journal.lastResult = Journal::Result::VirtualFileSystemFailed;
        journal.lastErrorMessage = "Unable to open " + fromPath(sourcePath);
        return false;
    }
}

constexpr uint32_t PatcherContribution = 512 * 1024 * 1024;

bool Installer::parseSources(const Input &input, Journal &journal, Sources &sources)
{
    journal = Journal();
    sources = Sources();

    // Parse the contents of the base game.
    if (!input.gameSource.empty())
    {
        if (!parseContent(input.gameSource, sources.game, journal))
        {
            return false;
        }

        if (!computeTotalSize({ GameFiles, GameFilesSize }, GameHashes, *sources.game, journal, sources.totalSize))
        {
            return false;
        }
    }

    // Parse the contents of Update.
    if (!input.updateSource.empty())
    {
        // Add an arbitrary progress size for the patching process.
        journal.progressTotal += PatcherContribution;

        if (!parseContent(input.updateSource, sources.update, journal))
        {
            return false;
        }

        if (!computeTotalSize({ UpdateFiles, UpdateFilesSize }, UpdateHashes, *sources.update, journal, sources.totalSize))
        {
            return false;
        }
    }

    // Parse the contents of the DLC Packs.
    for (const auto &path : input.dlcSources)
    {
        sources.dlc.emplace_back();
        DLCSource &dlcSource = sources.dlc.back();
        if (!parseContent(path, dlcSource.sourceVfs, journal))
        {
            return false;
        }

        DLC dlc = detectDLC(path, *dlcSource.sourceVfs, journal);
        if (!fillDLCSource(dlc, dlcSource))
        {
            return false;
        }

        if (!computeTotalSize(dlcSource.filePairs, dlcSource.fileHashes, *dlcSource.sourceVfs, journal, sources.totalSize))
        {
            return false;
        }
    }

    // Add the total size in bytes as the journal progress.
    journal.progressTotal += sources.totalSize;

    return true;
}

bool Installer::install(const Sources &sources, const std::filesystem::path &targetDirectory, bool skipHashChecks, Journal &journal, std::chrono::seconds endWaitTime, const std::function<bool()> &progressCallback)
{
    // Install files in reverse order of importance. In case of a process crash or power outage, this will increase the likelihood of the installation
    // missing critical files required for the game to run. These files are used as the way to detect if the game is installed.

    // Install the DLC.
    if (!sources.dlc.empty())
    {
        journal.createdDirectories.insert(targetDirectory / DLCDirectory);
    }

    for (const DLCSource &dlcSource : sources.dlc)
    {
        if (!copyFiles(dlcSource.filePairs, dlcSource.fileHashes, *dlcSource.sourceVfs, targetDirectory / dlcSource.targetSubDirectory, dlcSource.validationFile, skipHashChecks, journal, progressCallback))
        {
            return false;
        }
    }

    // If no game or update was specified, we're finished. This means the user was only installing the DLC.
    if ((sources.game == nullptr) && (sources.update == nullptr))
    {
        return true;
    }

    // Install the update.
    if (!copyFiles({ UpdateFiles, UpdateFilesSize }, UpdateHashes, *sources.update, targetDirectory / UpdateDirectory, UpdateExecutablePatchFile, skipHashChecks, journal, progressCallback))
    {
        return false;
    }

    // Install the base game.
    if (!copyFiles({ GameFiles, GameFilesSize }, GameHashes, *sources.game, targetDirectory / GameDirectory, GameExecutableFile, skipHashChecks, journal, progressCallback))
    {
        return false;
    }

    // Create the directory where the patched executable will be stored.
    std::error_code ec;
    std::filesystem::path patchedDirectory = targetDirectory / PatchedDirectory;
    if (!std::filesystem::exists(patchedDirectory) && !std::filesystem::create_directories(patchedDirectory, ec))
    {
        journal.lastResult = Journal::Result::DirectoryCreationFailed;
        journal.lastErrorMessage = "Unable to create directory at " + fromPath(patchedDirectory);
        return false;
    }

    journal.createdDirectories.insert(patchedDirectory);

    // Patch the executable with the update's file.
    std::filesystem::path baseXexPath = targetDirectory / GameDirectory / GameExecutableFile;
    std::filesystem::path patchPath = targetDirectory / UpdateDirectory / UpdateExecutablePatchFile;
    std::filesystem::path patchedXexPath = patchedDirectory / GameExecutableFile;
    XexPatcher::Result patcherResult = XexPatcher::apply(baseXexPath, patchPath, patchedXexPath);
    if (patcherResult == XexPatcher::Result::Success)
    {
        journal.createdFiles.push_back(patchedXexPath);
    }
    else
    {
        journal.lastResult = Journal::Result::PatchProcessFailed;
        journal.lastPatcherResult = patcherResult;
        journal.lastErrorMessage = "Patch process failed.";
        return false;
    }

    // Update the progress with the artificial amount attributed to the patching.
    journal.progressCounter += PatcherContribution;
    
    for (uint32_t i = 0; i < 2; i++)
    {
        if (!progressCallback())
        {
            journal.lastResult = Journal::Result::Cancelled;
            journal.lastErrorMessage = "Installation was cancelled.";
            return false;
        }

        if (i == 0)
        {
            // Wait the specified amount of time to allow the consumer of the callbacks to animate, halt or cancel the installation for a while after it's finished.
            std::this_thread::sleep_for(endWaitTime);
        }
    }

    return true;
}

void Installer::rollback(Journal &journal)
{
    std::error_code ec;
    for (const auto &path : journal.createdFiles)
    {
        std::filesystem::remove(path, ec);
    }

    for (auto it = journal.createdDirectories.rbegin(); it != journal.createdDirectories.rend(); it++)
    {
        std::filesystem::remove(*it, ec);
    }
}

bool Installer::parseGame(const std::filesystem::path &sourcePath)
{
    std::unique_ptr<VirtualFileSystem> sourceVfs = createFileSystemFromPath(sourcePath);
    if (sourceVfs == nullptr)
    {
        return false;
    }

    return sourceVfs->exists(GameExecutableFile);
}

bool Installer::parseUpdate(const std::filesystem::path &sourcePath)
{
    std::unique_ptr<VirtualFileSystem> sourceVfs = createFileSystemFromPath(sourcePath);
    if (sourceVfs == nullptr)
    {
        return false;
    }

    return sourceVfs->exists(UpdateExecutablePatchFile);
}

DLC Installer::parseDLC(const std::filesystem::path &sourcePath)
{
    Journal journal;
    std::unique_ptr<VirtualFileSystem> sourceVfs = createFileSystemFromPath(sourcePath);
    if (sourceVfs == nullptr)
    {
        return DLC::Unknown;
    }

    return detectDLC(sourcePath, *sourceVfs, journal);
}

XexPatcher::Result Installer::checkGameUpdateCompatibility(const std::filesystem::path &gameSourcePath, const std::filesystem::path &updateSourcePath)
{
    std::unique_ptr<VirtualFileSystem> gameSourceVfs = createFileSystemFromPath(gameSourcePath);
    if (gameSourceVfs == nullptr)
    {
        return XexPatcher::Result::FileOpenFailed;
    }

    std::unique_ptr<VirtualFileSystem> updateSourceVfs = createFileSystemFromPath(updateSourcePath);
    if (updateSourceVfs == nullptr)
    {
        return XexPatcher::Result::FileOpenFailed;
    }

    std::vector<uint8_t> xexBytes;
    std::vector<uint8_t> patchBytes;
    if (!gameSourceVfs->load(GameExecutableFile, xexBytes))
    {
        return XexPatcher::Result::FileOpenFailed;
    }

    if (!updateSourceVfs->load(UpdateExecutablePatchFile, patchBytes))
    {
        return XexPatcher::Result::FileOpenFailed;
    }

    std::vector<uint8_t> patchedBytes;
    return XexPatcher::apply(xexBytes.data(), xexBytes.size(), patchBytes.data(), patchBytes.size(), patchedBytes, true);
}
