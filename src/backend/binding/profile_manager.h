#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

namespace godot
{
    class ProfileManager
    {
        std::shared_ptr<spdlog::logger> m_logger;

        std::filesystem::path m_dir;

        std::vector<std::string> m_availableProfiles;

    public:
        ProfileManager(std::filesystem::path aDir, std::shared_ptr<spdlog::logger> aLogger);

        void Refresh();

        void Create(const std::string& aProfileName);
        void Delete(const std::string& aProfileName);
        void Duplicate(const std::string& aProfileName, const std::string& aDuplicatedName);

        void DeleteSelected();
        void DuplicateSelected(const std::string& aDuplicatedName);

        const std::vector<std::string> GetAvailableProfiles() const;

        void Select(const std::string& aProfileName);
        void Unselect();

        const std::string& GetSelected() const;
    };
}
