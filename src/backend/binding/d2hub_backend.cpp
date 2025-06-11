#include "d2hub_backend.h"

#include "d2/achievements/achievements.h"
#include "d2/utilities/setup.h"
#include "spdlog/sinks/daily_file_sink.h"

#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

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

bool D2HubBackend::IsMxlDirValid(const std::filesystem::path& aPath) const
{
    if (!aPath.is_absolute() || !std::filesystem::is_directory(aPath))
    {
        return false;
    }
    // cont
    return true;
}

std::shared_ptr<spdlog::logger> godot::D2HubBackend::MakeLogger(const std::string& aName) const
{
    auto logger = std::make_shared<spdlog::logger>(aName, m_commonFileSink);
    logger->set_level(m_logLevel);
    return logger;
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
    : m_developerControl(DeveloperControl::Create())
{
    if (OS::get_singleton()->get_cmdline_args().has("--debug"))
    {
        m_logLevel = spdlog::level::debug;
    }
    std::string logs_dir = godot::ProjectSettings::get_singleton()->globalize_path("user://logs").utf8().get_data();
    std::string log_file = logs_dir + "/d2hub.log";
    m_commonFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 3, 33, false, 5);
    m_logger = MakeLogger("d2hub_backend");

    m_achievementManager = std::make_unique<decltype(m_achievementManager)::element_type>(D2::CreateAchievements,
                                                                                          MakeLogger("achievement_manager"));

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
    Clear();
    auto targetProcessConfig = D2::CreateTargetProcessConfig(path);

    m_targetProcess = PMA::TargetProcess::Create(targetProcessConfig, MakeLogger("target_process"));
    m_targetProcessExistenceToken = m_targetProcess->OnExistenceChanged([this](bool aTargetProcessExists) {
        call_deferred("emit_signal", "target_process_exists", aTargetProcessExists);
    });
    m_targetProcessAttachmentToken = m_targetProcess->OnAttachmentChanged([this](bool aTargetProcessAttached) {
        call_deferred("emit_signal", "target_process_attached", aTargetProcessAttached);
    });
    m_memoryProcessor = GE::MemoryProcessor::Create(m_targetProcess, MakeLogger("memory_processor"));

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
