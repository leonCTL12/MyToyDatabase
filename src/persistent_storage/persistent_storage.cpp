#include "persistent_storage.h"
#include <iostream>
#include <fstream>

bool PersistentStorage::fileExists(const fs::path &path) const
{
    return fs::exists(path);
}

bool PersistentStorage::tryCreateFolder(const fs::path &path) const
{
    if (fs::exists(path))
    {
        return false; // Directory already exists
    }
    try
    {
        return fs::create_directories(path);
    }
    catch (const fs::filesystem_error &e)
    {
        std::cout << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

bool PersistentStorage::tryCreateFile(const fs::path &path) const
{
    if (fs::exists(path))
    {
        return false; // File already exists
    }
    try
    {
        // Attempt to create the file
        std::ofstream file(path);

        if (file)
        {
            file.close();
            return true;
        }
        else
        {
            std::cout << "Error creating file: " << path << std::endl;
            return false;
        }
    }
    catch (const std::ios_base::failure &e)
    {
        std::cout << "Error creating file: " << e.what() << std::endl;
        return false;
    }
}