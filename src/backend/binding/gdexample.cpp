#include "gdexample.h"

#include "d2/achievements/achievements.h"
#include "d2/utilities/setup.h"

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
    ClassDB::bind_method(D_METHOD("get_conditions"), &AchievementConditions::get_conditions);
}

Ref<AchievementConditions> AchievementConditions::FromAchievement(const D2::D2Achi& aAchi)
{
    Ref<AchievementConditions> instance = memnew(AchievementConditions);
    for (int i = 0; i < static_cast<int>(GE::ConditionType::All); ++i)
    {
        auto c = static_cast<GE::ConditionType>(i);
        const auto& names = aAchi->GetConditionNames(c);
        const auto& results = aAchi->GetConditionResults(c);
        Dictionary dict = (instance->m_conditions[GE::to_string(c).c_str()] = Dictionary());
        for (size_t i = 0; i < names.size(); ++i)
        {
            dict["names"] = String(names[i].c_str());
            dict["results"] = results[i];
        }
    }
    return instance;
}

Dictionary AchievementConditions::get_conditions() const
{
    return m_conditions;
}

bool GDExample::CanUpdate() const
{
    return m_dataAccess && m_sharedData;
}

void GDExample::Update()
{
    m_dataAccess->AdvanceFrame();
    m_sharedData->Update();
    m_achievementManager->Update(*m_dataAccess, *m_sharedData);
}

void GDExample::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_achievements"), &GDExample::get_achievements);
    ClassDB::bind_method(D_METHOD("initialize_backend", "p_path_to_modules"), &GDExample::initialize_backend);
    ClassDB::bind_method(D_METHOD("discover_target_process"), &GDExample::discover_target_process);
    ClassDB::bind_method(D_METHOD("attach_to_target_process", "p_attach"), &GDExample::attach_to_target_process);

    ADD_SIGNAL(MethodInfo("target_process_exists", PropertyInfo(Variant::BOOL, "exists")));
    ADD_SIGNAL(MethodInfo("target_process_attached", PropertyInfo(Variant::BOOL, "attached")));
}

GDExample::GDExample()
    : m_achievementManager(std::make_unique<decltype(m_achievementManager)::element_type>(D2::CreateAchievements))
{
    // TODO tmp -> this just creates default achievements, no loading from file
    std::istringstream invalid;
    invalid.setstate(std::ios::failbit);
    m_achievementManager->LoadAndActivate(invalid);
    for (const auto& [_, achi] : m_achievementManager->GetActiveAchievements())
    {
        m_achievements.push_back(Achievement::FromAchievement(achi));
    }
}

GDExample::~GDExample() {}

void GDExample::_process(double delta) {}

Array GDExample::get_achievements()
{
    return m_achievements;
}

void GDExample::initialize_backend(const String& path_to_modules)
{
    m_memoryProcessor.reset();
    m_targetProcess.reset();
    if (!path_to_modules.is_absolute_path())
    {
        return;
    }
    auto targetProcessConfig = D2::CreateTargetProcessConfig(reinterpret_cast<const char8_t*>(path_to_modules.utf8().get_data()));

    m_targetProcess = PMA::TargetProcess::Create(targetProcessConfig);
    m_targetProcessExistenceToken = m_targetProcess->OnExistenceChanged([this](bool aTargetProcessExists) {
        call_deferred("emit_signal", "target_process_exists", aTargetProcessExists);
    });
    m_targetProcessAttachmentToken = m_targetProcess->OnAttachmentChanged([this](bool aTargetProcessAttached) {
        call_deferred("emit_signal", "target_process_attached", aTargetProcessAttached);
    });
    m_memoryProcessor = GE::MemoryProcessor::Create(m_targetProcess);

    D2::RegisterLayouts(*m_memoryProcessor);
    D2::SetupCallbacks(*m_memoryProcessor, m_dataAccess, m_sharedData);

    m_memoryProcessor->SetUpdateCallback([this](const GE::DataAccessor&) {
        if (!CanUpdate())
        {
            return;
        }
        Update();
        // TODO rest... or make it via callbacks
    });
}

void GDExample::discover_target_process()
{
    m_targetProcess->Discover();
}

void GDExample::attach_to_target_process(bool attach)
{
    if (attach)
    {
        m_targetProcess->Attach();
    }
    else
    {
        m_targetProcess->Detach();
    }
}

void Achievement::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_metadata"), &Achievement::get_metadata);
    ClassDB::bind_method(D_METHOD("get_conditions"), &Achievement::get_conditions);
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
