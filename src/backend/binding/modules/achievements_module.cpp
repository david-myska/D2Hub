#include "achievements_module.h"

#include <filesystem>
#include <memory>

#include "d2/achievements/achievements.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Dictionary CreateMetadataFromAchievement(const D2::D2Achi& aAchi)
{
    Dictionary metadata;
    metadata["name"] = String(aAchi->GetMetadata().m_name.c_str());
    metadata["description"] = String(aAchi->GetMetadata().m_description.c_str());
    metadata["icon"] = String(aAchi->GetMetadata().m_icon.c_str());
    metadata["category"] = String(aAchi->GetMetadata().m_category.c_str());
    return metadata;
}

void AchievementConditions::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_conditions_by_ids"), &AchievementConditions::get_conditions_by_ids);
    ClassDB::bind_method(D_METHOD("get_conditions_by_categories"), &AchievementConditions::get_conditions_by_categories);
}

Ref<AchievementConditions> AchievementConditions::FromAchievement(const D2::D2Achi& aAchi)
{
    Ref<AchievementConditions> instance = memnew(AchievementConditions);
    for (int i = 0; i < static_cast<int>(GE::ConditionType::All); ++i)
    {
        auto c = static_cast<GE::ConditionType>(i);
        const auto& progressTrackers = aAchi->GetProgress(c);
        Dictionary category;
        for (const auto* pt : progressTrackers)
        {
            // Frontend stores some additional data in the relatedData dictionary on initialization
            Dictionary relatedData;
            relatedData["text"] = String(pt->GetMessage().c_str());
            relatedData["completed"] = pt->IsCompleted();
            relatedData["type"] = i;
            category[pt->GetId()] = relatedData;
            instance->m_conditionsByIds[pt->GetId()] = relatedData;
        }
        instance->m_conditionsByCategories[i] = category;
    }
    return instance;
}

Dictionary AchievementConditions::get_conditions_by_ids() const
{
    return m_conditionsByIds;
}

Dictionary AchievementConditions::get_conditions_by_categories() const
{
    return m_conditionsByCategories;
}

void Achievement::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_status"), &Achievement::get_status);
    ClassDB::bind_method(D_METHOD("get_metadata"), &Achievement::get_metadata);
    ClassDB::bind_method(D_METHOD("get_conditions"), &Achievement::get_conditions);

    ClassDB::bind_integer_constant("Achievement", "Status", "DISABLED", static_cast<int>(GE::Status::Disabled));
    ClassDB::bind_integer_constant("Achievement", "Status", "INACTIVE", static_cast<int>(GE::Status::Inactive));
    ClassDB::bind_integer_constant("Achievement", "Status", "ACTIVE", static_cast<int>(GE::Status::Active));
    ClassDB::bind_integer_constant("Achievement", "Status", "COMPLETED", static_cast<int>(GE::Status::Completed));
    ClassDB::bind_integer_constant("Achievement", "Status", "FAILED", static_cast<int>(GE::Status::Failed));
    ClassDB::bind_integer_constant("Achievement", "Status", "ALL_STATUSES", static_cast<int>(GE::Status::All));

    ClassDB::bind_integer_constant("Achievement", "Condition", "PRECONDITION", static_cast<int>(GE::ConditionType::Precondition));
    ClassDB::bind_integer_constant("Achievement", "Condition", "ACTIVATOR", static_cast<int>(GE::ConditionType::Activator));
    ClassDB::bind_integer_constant("Achievement", "Condition", "COMPLETER", static_cast<int>(GE::ConditionType::Completer));
    ClassDB::bind_integer_constant("Achievement", "Condition", "FAILER", static_cast<int>(GE::ConditionType::Failer));
    ClassDB::bind_integer_constant("Achievement", "Condition", "VALIDATOR", static_cast<int>(GE::ConditionType::Validator));
    ClassDB::bind_integer_constant("Achievement", "Condition", "RESETER", static_cast<int>(GE::ConditionType::Reseter));
    ClassDB::bind_integer_constant("Achievement", "Condition", "ALL_CONDITIONS", static_cast<int>(GE::ConditionType::All));

    ADD_SIGNAL(MethodInfo("status_changed", PropertyInfo(Variant::INT, "status")));
    ADD_SIGNAL(MethodInfo("progress_made", PropertyInfo(Variant::ARRAY, "ids")));
}

Ref<Achievement> Achievement::FromAchievement(const D2::D2Achi& aAchi)
{
    auto godotAchi = memnew(Achievement);
    godotAchi->m_status = aAchi->GetStatus();
    godotAchi->m_metadata = CreateMetadataFromAchievement(aAchi);
    godotAchi->m_conditions = AchievementConditions::FromAchievement(aAchi);
    godotAchi->m_onStatusChangedToken = aAchi->OnStatusChanged([godotAchi](GE::Status aNew, GE::Status) {
        godotAchi->m_status = aNew;
        godotAchi->call_deferred("emit_signal", "status_changed", static_cast<int>(aNew));
    });
    godotAchi->m_onProgressMadeToken = aAchi->OnProgressMade(
        [godotAchi](const std::unordered_set<GE::ProgressTracker*>& aProgress) {
            auto conditionsByIds = godotAchi->get_conditions()->get_conditions_by_ids();
            Array ids;
            for (const auto* pt : aProgress)
            {
                Dictionary data = conditionsByIds[pt->GetId()];
                data["text"] = String(pt->GetMessage().c_str());
                data["completed"] = pt->IsCompleted();
                ids.push_back(pt->GetId());
            }
            godotAchi->call_deferred("emit_signal", "progress_made", ids);
        });

    return godotAchi;
}

Dictionary Achievement::get_metadata() const
{
    return m_metadata;
}

Ref<AchievementConditions> Achievement::get_conditions() const
{
    return m_conditions;
}

int Achievement::get_status() const
{
    return static_cast<int>(m_status);
}

void AchievementsModule::UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    if (!m_achievementManager)
    {
        m_logger->warn("Achievement manager is not initialized, cannot update achievements.");
        return;
    }
    m_achievementManager->Update(aDataAccess, aSharedData);
}

void AchievementsModule::LoadAchievements(std::optional<std::string> aId, bool aActivate)
{
    m_achievements.clear();
    auto loadedAchievements = m_achievementManager->Load(std::move(aId));
    for (const auto& [_, achi] : loadedAchievements)
    {
        m_achievements.push_back(Achievement::FromAchievement(achi));
    }
    if (aActivate)
    {
        m_achievementManager->Activate(std::move(loadedAchievements));
    }
    call_deferred("emit_signal", "new_achievements_loaded");
}

void AchievementsModule::SaveAchievements(const std::string& aId)
{
    if (!m_achievementManager)
    {
        m_logger->warn("Achievement manager is not initialized, cannot save achievements.");
        return;
    }
    m_achievementManager->Save(aId);
}

void AchievementsModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_achievements"), &AchievementsModule::get_achievements);

    ADD_SIGNAL(MethodInfo("new_achievements_loaded"));
}

Ref<AchievementsModule> AchievementsModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(AchievementsModule);
    module->m_logger = aLogger;
    module->m_notifier = std::move(aNotifier);
    module->m_name = "Achievements";
    module->SetUserDir("achievements");
    module->m_achievementManager = std::make_unique<decltype(module->m_achievementManager)::element_type>(
        D2::CreateAchievements, module->m_moduleUserDir, std::move(aLogger));
    return module;
}

Array AchievementsModule::get_achievements()
{
    return m_achievements;
}
