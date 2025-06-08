#include "d2hub_backend.h"

#include "d2/achievements/achievements.h"
#include "d2/utilities/setup.h"
#include "pma/logging/console_logger.h"

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

bool D2HubBackend::CanUpdate() const
{
    return m_dataAccess && m_sharedData;
}

void D2HubBackend::Update()
{
    m_dataAccess->AdvanceFrame();
    m_sharedData->Update();
    m_achievementManager->Update(*m_dataAccess, *m_sharedData);
}

void D2HubBackend::Clear()
{
    m_dataAccess.reset();
    m_sharedData.reset();
    m_memoryProcessor.reset();
    m_targetProcess.reset();
    m_targetProcessAttachmentToken.reset();
    m_targetProcessExistenceToken.reset();
}

bool D2HubBackend::IsMxlDirValid(const std::filesystem::path& path) const
{
    if (!path.is_absolute() || !std::filesystem::is_directory(path))
    {
        return false;
    }
    // cont
    return true;
}

void D2HubBackend::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("is_mxl_dir_valid", "p_path"), &D2HubBackend::is_mxl_dir_valid);
    ClassDB::bind_method(D_METHOD("get_achievements"), &D2HubBackend::get_achievements);
    ClassDB::bind_method(D_METHOD("get_developer_control"), &D2HubBackend::get_developer_control);
    ClassDB::bind_method(D_METHOD("initialize_backend", "p_path_to_modules"), &D2HubBackend::initialize_backend);
    ClassDB::bind_method(D_METHOD("discover_target_process"), &D2HubBackend::discover_target_process);
    ClassDB::bind_method(D_METHOD("attach_to_target_process", "p_attach"), &D2HubBackend::attach_to_target_process);
    ClassDB::bind_method(D_METHOD("start_memory_processor"), &D2HubBackend::start_memory_processor);
    ClassDB::bind_method(D_METHOD("stop_memory_processor"), &D2HubBackend::stop_memory_processor);

    ADD_SIGNAL(MethodInfo("backend_initialized", PropertyInfo(Variant::BOOL, "initialized")));
    ADD_SIGNAL(MethodInfo("target_process_exists", PropertyInfo(Variant::BOOL, "exists")));
    ADD_SIGNAL(MethodInfo("target_process_attached", PropertyInfo(Variant::BOOL, "attached")));
    ADD_SIGNAL(MethodInfo("target_memory_processing", PropertyInfo(Variant::BOOL, "processing")));
}

D2HubBackend::D2HubBackend()
    : m_achievementManager(std::make_unique<decltype(m_achievementManager)::element_type>(D2::CreateAchievements))
    , m_developerControl(DeveloperControl::Create())
{
    // TODO tmp -> this just creates default achievements, no loading from file
    std::istringstream invalid;
    invalid.setstate(std::ios::failbit);
    m_achievementManager->LoadAndActivate(invalid);
    for (const auto& [_, achi] : m_achievementManager->GetActiveAchievements())
    {
        auto godotAchi = Achievement::FromAchievement(achi);
        achi->OnStatusChanged([godotAchi](GE::Status aStatus) {
            godotAchi->call_deferred("emit_signal", "status_changed", static_cast<int>(aStatus));
        });
        achi->OnProgressMade([godotAchi](const std::unordered_set<GE::ProgressTracker*>& aProgress) {
            auto conditionsByIds = godotAchi->get_conditions()->get_conditions_by_ids();
            Array ids;
            for (const auto* pt : aProgress)
            {
                // Ref<Dictionary> data = conditionsByIds[pt->GetId()];
                // data->set("text", String(pt->GetMessage().c_str()));
                Dictionary data = conditionsByIds[pt->GetId()];
                data["text"] = String(pt->GetMessage().c_str());
                ids.push_back(pt->GetId());
            }
            godotAchi->call_deferred("emit_signal", "progress_made", ids);
        });
        m_achievements.push_back(std::move(godotAchi));
    }
}

D2HubBackend::~D2HubBackend() {}

void D2HubBackend::_process(double delta) {}

Array D2HubBackend::get_achievements()
{
    return m_achievements;
}

Ref<DeveloperControl> godot::D2HubBackend::get_developer_control()
{
    return m_developerControl;
}

struct GodotLogger : public PMA::ILogger
{
    void Log(PMA::LogLevel, const std::string& msg) const override { UtilityFunctions::print(msg.c_str()); }
};

bool D2HubBackend::is_mxl_dir_valid(const String& path) const
{
    return IsMxlDirValid(std::filesystem::path(reinterpret_cast<const char8_t*>(path.utf8().get_data())));
}

void D2HubBackend::initialize_backend(const String& path_to_modules)
{
    auto path = std::filesystem::path(reinterpret_cast<const char8_t*>(path_to_modules.utf8().get_data()));
    if (!IsMxlDirValid(path))
    {
        return;
    }
    PMA::Setup::InjectLogger(std::make_unique<GodotLogger>(), PMA::LogLevel::Info);
    Clear();
    auto targetProcessConfig = D2::CreateTargetProcessConfig(path);

    m_targetProcess = PMA::TargetProcess::Create(targetProcessConfig);
    m_targetProcessExistenceToken = m_targetProcess->OnExistenceChanged([this](bool aTargetProcessExists) {
        call_deferred("emit_signal", "target_process_exists", aTargetProcessExists);
    });
    m_targetProcessAttachmentToken = m_targetProcess->OnAttachmentChanged([this](bool aTargetProcessAttached) {
        call_deferred("emit_signal", "target_process_attached", aTargetProcessAttached);
    });
    m_memoryProcessor = GE::MemoryProcessor::Create(m_targetProcess);

    D2::RegisterLayouts(*m_memoryProcessor);
    D2::SetupCallbacks(
        *m_memoryProcessor,
        [this](std::shared_ptr<GE::DataAccessor> aDataAccessor) {
            m_dataAccess = std::make_shared<D2::Data::DataAccess>(aDataAccessor);
            m_sharedData = std::make_shared<D2::Data::SharedData>(m_dataAccess);
            m_developerControl->Initialize(m_dataAccess, m_sharedData);
            UtilityFunctions::print("In-game data is ready.");
        },
        [this]() {
            m_dataAccess.reset();
            m_sharedData.reset();
        });

    m_memoryProcessor->SetUpdateCallback([this](const GE::DataAccessor&) {
        if (!CanUpdate())
        {
            return;
        }
        Update();
    });
    call_deferred("emit_signal", "backend_initialized", true);
}

void D2HubBackend::discover_target_process()
{
    if (!m_targetProcess)
    {
        return;
    }
    m_targetProcess->Discover();
}

void D2HubBackend::attach_to_target_process(bool attach)
{
    if (!m_targetProcess)
    {
        return;
    }
    if (attach)
    {
        m_targetProcess->Attach();
    }
    else
    {
        m_targetProcess->Detach();
    }
}

void D2HubBackend::start_memory_processor()
{
    if (!m_memoryProcessor)
    {
        return;
    }
    try
    {
        m_memoryProcessor->RequestStart();
    }
    catch (const std::exception& e)
    {
        // TODO log
    }
}

void D2HubBackend::stop_memory_processor()
{
    if (!m_memoryProcessor)
    {
        return;
    }
    m_memoryProcessor->RequestStop();
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

void DeveloperControl::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_player_position"), &DeveloperControl::get_player_position);
}

Ref<DeveloperControl> DeveloperControl::Create()
{
    auto dev = memnew(DeveloperControl);
    return dev;
}

void DeveloperControl::Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared)
{
    m_data = std::move(aData);
    m_shared = std::move(aShared);
}

Vector2i DeveloperControl::get_player_position() const
{
    auto pos = m_data->GetPlayers().GetLocal()->m_pos;
    return Vector2i(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}
