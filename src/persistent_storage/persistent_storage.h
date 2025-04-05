#pragma once
#include <filesystem>

namespace fs = std::filesystem;
class PersistentStorage
{
public:
    bool fileExists(const fs::path &path) const;
    bool tryCreateFolder(const fs::path &path) const;
    bool tryCreateFile(const fs::path &path) const;
    bool tryDeleteFolder(const fs::path &path) const;
};