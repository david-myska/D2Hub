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
    m_achievementsModule->Update(*m_dataAccess, *m_sharedData);
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

spdlog::level::level_enum godot::D2HubBackend::ParseLogLevel()
{
    if (OS::get_singleton()->get_cmdline_args().has("--debug"))
    {
        return spdlog::level::debug;
    }
    return spdlog::level::info;
}

std::shared_ptr<spdlog::sinks::sink> D2HubBackend::MakeLoggerSink()
{
    std::string user_dir = godot::ProjectSettings::get_singleton()->globalize_path("user://").utf8().get_data();
    std::string log_file = user_dir + "/logs/d2hub.log";
    return std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 3, 33, false, 5);
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
    ClassDB::bind_method(D_METHOD("initialize_backend", "p_path_to_modules"), &D2HubBackend::initialize_backend);
    ClassDB::bind_method(D_METHOD("discover_target_process"), &D2HubBackend::discover_target_process);
    ClassDB::bind_method(D_METHOD("attach_to_target_process", "p_attach"), &D2HubBackend::attach_to_target_process);
    ClassDB::bind_method(D_METHOD("start_memory_processor"), &D2HubBackend::start_memory_processor);
    ClassDB::bind_method(D_METHOD("stop_memory_processor"), &D2HubBackend::stop_memory_processor);

    ClassDB::bind_method(D_METHOD("get_achievements_module"), &D2HubBackend::get_achievements_module);
    ClassDB::bind_method(D_METHOD("get_backup_module"), &D2HubBackend::get_backup_module);
    ClassDB::bind_method(D_METHOD("get_developer_module"), &D2HubBackend::get_developer_module);

    ADD_SIGNAL(MethodInfo("backend_initialized", PropertyInfo(Variant::BOOL, "initialized")));
    ADD_SIGNAL(MethodInfo("target_process_exists", PropertyInfo(Variant::BOOL, "exists")));
    ADD_SIGNAL(MethodInfo("target_process_attached", PropertyInfo(Variant::BOOL, "attached")));
    ADD_SIGNAL(MethodInfo("memory_processor_running", PropertyInfo(Variant::BOOL, "processing")));
    ADD_SIGNAL(MethodInfo("show_popup", PropertyInfo(Variant::STRING, "message")));
}

D2HubBackend::D2HubBackend()
    : m_logLevel(ParseLogLevel())
    , m_commonFileSink(MakeLoggerSink())
    , m_logger(MakeLogger("d2hub_backend"))
    , m_achievementsModule(AchievementsModule::Create(MakeLogger("achievements_module")))
    , m_backupModule(BackupModule::Create(MakeLogger("backup_module")))
    , m_developerModule(DeveloperModule::Create(MakeLogger("developer_module")))
{
    m_achievementsModule->LoadAchievements({}, false);
}

D2HubBackend::~D2HubBackend() {}

Ref<AchievementsModule> D2HubBackend::get_achievements_module()
{
    return m_achievementsModule;
}

Ref<BackupModule> D2HubBackend::get_backup_module()
{
    return m_backupModule;
}

Ref<DeveloperModule> D2HubBackend::get_developer_module()
{
    return m_developerModule;
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
    m_memoryProcessorRunningToken = m_memoryProcessor->OnRunningChanged([this](bool aRunning) {
        call_deferred("emit_signal", "memory_processor_running", aRunning);
    });

    D2::RegisterLayouts(*m_memoryProcessor);
    D2::SetupCallbacks(
        *m_memoryProcessor,
        [this](std::shared_ptr<GE::DataAccessor> aDataAccessor) {
            m_backupModule->AutoBackup();
            m_dataAccess = std::make_shared<D2::Data::DataAccess>(aDataAccessor);
            m_sharedData = std::make_shared<D2::Data::SharedData>(m_dataAccess);
            m_developerModule->Initialize(m_dataAccess, m_sharedData);
            m_achievementsModule->LoadAchievements(m_dataAccess->GetLocalPlayerName(), !D2::InvalidStart());
        },
        [this]() {
            m_achievementsModule->SaveAchievements(m_dataAccess->GetLocalPlayerName());
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
