#include "achievement.h"

#include "d2/achievements/achievements.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void AchievementMetadata::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_name"), &AchievementMetadata::get_name);
    ClassDB::bind_method(D_METHOD("set_name", "name"), &AchievementMetadata::set_name);
    ClassDB::bind_method(D_METHOD("get_description"), &AchievementMetadata::get_description);
    ClassDB::bind_method(D_METHOD("set_description", "description"), &AchievementMetadata::set_description);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
}

Ref<AchievementMetadata> AchievementMetadata::FromAchievement(const D2::D2Achi& aAchi)
{
    const auto& meta = aAchi->GetMetadata();
    Ref<AchievementMetadata> instance = memnew(AchievementMetadata);
    instance->set_name(meta.m_name.c_str());
    instance->set_description(meta.m_description.c_str());
    return instance;
}

String AchievementMetadata::get_name() const
{
    return m_name;
}

void AchievementMetadata::set_name(const String& name)
{
    m_name = name;
}

const String& AchievementMetadata::get_description() const
{
    return m_description;
}

void AchievementMetadata::set_description(const String& description)
{
    m_description = description;
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
            // Ref to make only 1 instance that gets synchronized
            // Ref<Dictionary> relatedData = memnew(Dictionary);
            // relatedData->set("text", String(pt->GetMessage().c_str()));
            Dictionary relatedData;
            relatedData["text"] = String(pt->GetMessage().c_str());
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
    auto achievement = memnew(Achievement);
    achievement->m_metadata = AchievementMetadata::FromAchievement(aAchi);
    achievement->m_conditions = AchievementConditions::FromAchievement(aAchi);
    return achievement;
}

Ref<AchievementMetadata> Achievement::get_metadata() const
{
    return m_metadata;
}

Ref<AchievementConditions> Achievement::get_conditions() const
{
    return m_conditions;
}
