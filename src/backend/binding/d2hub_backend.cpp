#include "d2hub_backend.h"

#include "d2/achievements/achievements.h"
#include "d2/utilities/loaded_data.h"
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
    try
    {
        m_dataAccess->AdvanceFrame();
        m_sharedData->Update();
        m_achievementsModule->Update(*m_dataAccess, *m_sharedData);
        m_lootfilterModule->Update(*m_dataAccess, *m_sharedData);
        m_statisticsModule->Update(*m_dataAccess, *m_sharedData);
    }
    catch (const std::exception& e)
    {
        m_logger->error("Error during update: {}", e.what());
    }
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

// NOT WORKING PROPERLY
std::string D2HubBackend::GetMedianXlVersion() const
{
    auto versionFile = m_targetProcess->GetInfo().executable.parent_path() / "medianxl-version.mpq";
    std::array<char, 16> version = {0};
    std::ifstream f(versionFile, std::ios::binary);
    f.seekg(40).read(version.data(), 6);
    return version.data();
}

spdlog::level::level_enum D2HubBackend::ParseLogLevel()
{
    if (OS::get_singleton()->get_cmdline_args().has("--debug"))
    {
        return spdlog::level::debug;
    }
    return spdlog::level::info;
}

std::shared_ptr<spdlog::sinks::sink> D2HubBackend::MakeLoggerSink()
{
    std::string user_dir = ProjectSettings::get_singleton()->globalize_path("user://").utf8().get_data();
    std::string log_file = user_dir + "/logs/d2hub.log";
    return std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 3, 33, false, 5);
}

std::shared_ptr<spdlog::logger> D2HubBackend::MakeLogger(const std::string& aName) const
{
    auto logger = std::make_shared<spdlog::logger>(aName, m_commonFileSink);
    logger->set_level(m_logLevel);
    return logger;
}

void D2HubBackend::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("start_auto_attach"), &D2HubBackend::start_auto_attach);
    ClassDB::bind_method(D_METHOD("stop_auto_attach"), &D2HubBackend::stop_auto_attach);
    ClassDB::bind_method(D_METHOD("discover_target_process"), &D2HubBackend::discover_target_process);
    ClassDB::bind_method(D_METHOD("attach_to_target_process", "p_attach"), &D2HubBackend::attach_to_target_process);
    ClassDB::bind_method(D_METHOD("start_memory_processor"), &D2HubBackend::start_memory_processor);
    ClassDB::bind_method(D_METHOD("stop_memory_processor"), &D2HubBackend::stop_memory_processor);

    ClassDB::bind_method(D_METHOD("get_notifier"), &D2HubBackend::get_notifier);

    ClassDB::bind_method(D_METHOD("get_achievements_module"), &D2HubBackend::get_achievements_module);
    ClassDB::bind_method(D_METHOD("get_backup_module"), &D2HubBackend::get_backup_module);
    ClassDB::bind_method(D_METHOD("get_developer_module"), &D2HubBackend::get_developer_module);
    ClassDB::bind_method(D_METHOD("get_lootfilter_module"), &D2HubBackend::get_lootfilter_module);
    ClassDB::bind_method(D_METHOD("get_statistics_module"), &D2HubBackend::get_statistics_module);
    ClassDB::bind_method(D_METHOD("get_modules"), &D2HubBackend::get_modules);

    ClassDB::bind_method(D_METHOD("get_target_rect"), &D2HubBackend::get_target_rect);
    ClassDB::bind_method(D_METHOD("enable_window_clickthrough", "p_hwnd", "p_enable"), &D2HubBackend::enable_window_clickthrough);

    ClassDB::bind_method(D_METHOD("fucking_flush"), &D2HubBackend::fucking_flush);

    ADD_SIGNAL(MethodInfo("target_process_exists", PropertyInfo(Variant::BOOL, "exists")));
    ADD_SIGNAL(MethodInfo("target_process_attached", PropertyInfo(Variant::BOOL, "attached")));
    ADD_SIGNAL(MethodInfo("memory_processor_running", PropertyInfo(Variant::BOOL, "processing")));
}

D2HubBackend::D2HubBackend()
    : m_logLevel(ParseLogLevel())
    , m_commonFileSink(MakeLoggerSink())
    , m_logger(MakeLogger("d2hub_backend"))
    , m_notifier(Notifier::Create(MakeLogger("notifier")))
    , m_achievementsModule(AchievementsModule::Create(MakeLogger("achievements_module"), m_notifier))
    , m_backupModule(BackupModule::Create(MakeLogger("backup_module"), m_notifier))
    , m_developerModule(DeveloperModule::Create(MakeLogger("developer_module"), m_notifier))
    , m_lootfilterModule(LootFilterModule::Create(MakeLogger("lootfilter_module"), m_notifier))
    , m_statisticsModule(StatisticsModule::Create(MakeLogger("statistics_module"), m_notifier))
    , m_modules({m_achievementsModule, m_backupModule, m_developerModule, m_lootfilterModule, m_statisticsModule})
{
    D2::Data::LoadStats();
    D2::Data::LoadItems();
    InitializeBackend();
    m_achievementsModule->LoadAchievements({}, false);
    m_lootfilterModule->Load();
}

D2HubBackend::~D2HubBackend()
{
    m_logger->info("Destroying D2Hub backend");
    m_lootfilterModule->Save();
    m_commonFileSink->flush();
}

Ref<Notifier> D2HubBackend::get_notifier() const
{
    return m_notifier;
}

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

Ref<LootFilterModule> D2HubBackend::get_lootfilter_module()
{
    return m_lootfilterModule;
}

Ref<StatisticsModule> D2HubBackend::get_statistics_module()
{
    return m_statisticsModule;
}

Array D2HubBackend::get_modules() const
{
    return m_modules;
}

void D2HubBackend::InitializeBackend()
{
    m_logger->info("Initializing D2Hub backend");
    Clear();
    auto targetProcessConfig = D2::CreateTargetProcessConfig();

    auto pmaLogger = MakeLogger("pma");
    m_targetProcess = PMA::TargetProcess::Create(targetProcessConfig, pmaLogger);
    m_autoAttach = PMA::AutoAttach::Create(m_targetProcess, std::move(pmaLogger));
    m_targetProcessExistenceToken = m_targetProcess->OnExistenceChanged([this](bool aTargetProcessExists) {
        call_deferred("emit_signal", "target_process_exists", aTargetProcessExists);
        if (aTargetProcessExists)
        {
            m_logger->info("MXL version: {}", GetMedianXlVersion());
        }
    });
    m_targetProcessAttachmentToken = m_targetProcess->OnAttachmentChanged([this](bool aTargetProcessAttached) {
        call_deferred("emit_signal", "target_process_attached", aTargetProcessAttached);
    });
    m_memoryProcessor = GE::MemoryProcessor::Create(MakeLogger("memory_processor"));
    m_memoryProcessorRunningToken = m_memoryProcessor->OnRunningChanged([this](bool aRunning) {
        call_deferred("emit_signal", "memory_processor_running", aRunning);
    });
    // TODO only set this callback when autoattach is used, causes crashes when manually attaching to a process
    // m_startOnAttachedToken = m_targetProcess->OnAttachmentChanged([this](bool aAttached) {
    //    if (aAttached)
    //    {
    //        m_memoryProcessor->RequestStart(m_targetProcess->GetMemoryAccess());
    //    }
    //    else
    //    {
    //        m_memoryProcessor->RequestStop();
    //    }
    //});

    D2::RegisterLayouts(*m_memoryProcessor);
    D2::SetupCallbacks(
        *m_memoryProcessor,
        [this](std::shared_ptr<GE::DataAccessor> aDataAccessor) {
            m_backupModule->AutoBackup();
            m_dataAccess = std::make_shared<D2::Data::DataAccess>(aDataAccessor);
            m_sharedData = std::make_shared<D2::Data::SharedData>(m_dataAccess);
            m_achievementsModule->DisableProgramatically(D2::InvalidStart(), "Not in main menu on D2Hub start-up");
            m_developerModule->DisableProgramatically(false);
            m_lootfilterModule->DisableProgramatically(false);
            m_statisticsModule->DisableProgramatically(false);
        },
        [this]() {
            m_achievementsModule->DisableProgramatically(true, "In main menu, not in game");
            m_developerModule->DisableProgramatically(true, "In main menu, not in game");
            m_lootfilterModule->DisableProgramatically(true, "In main menu, not in game");
            m_statisticsModule->DisableProgramatically(true, "In main menu, not in game");
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
}

void D2HubBackend::start_auto_attach()
{
    m_autoAttach->Start();
}

void D2HubBackend::stop_auto_attach()
{
    m_autoAttach->Stop();
}

void D2HubBackend::discover_target_process()
{
    m_targetProcess->Discover();
}

void D2HubBackend::attach_to_target_process(bool attach)
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

void D2HubBackend::start_memory_processor()
{
    try
    {
        if (!m_targetProcess->IsAttached())
        {
            if (m_targetProcess->Discover())
            {
                m_targetProcess->Attach();
            }
        }
        if (m_targetProcess->IsAttached())
        {
            m_memoryProcessor->RequestStart(m_targetProcess->GetMemoryAccess());
        }
        else
        {
            m_logger->warn("Cannot start memory processor: TargetProcess is not attached.");
        }
    }
    catch (const std::exception& e)
    {
        m_logger->warn("Failed to start memory processor: {}", e.what());
    }
}

void D2HubBackend::stop_memory_processor()
{
    try
    {
        m_memoryProcessor->RequestStop();
    }
    catch (const std::exception& e)
    {
        m_logger->warn("Failed to stop memory processor: {}", e.what());
    }
}

#include <windows.h>

Rect2i D2HubBackend::get_target_rect() const
{
    auto w = FindWindowA(nullptr, "Diablo II");
    if (w)
    {
        RECT rect;
        if (GetWindowRect(w, &rect))
        {
            int virtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int virtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            int x = rect.left - virtualLeft;
            int y = rect.top - virtualTop;
            return Rect2i(x, y, width, height);
        }
    }
    return Rect2i();
}

void D2HubBackend::enable_window_clickthrough(uintptr_t godot_hwnd, bool enable)
{
    HWND hwnd = reinterpret_cast<HWND>(godot_hwnd);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    if (enable)
    {
        exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
    }
    else
    {
        exStyle &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
}

void D2HubBackend::fucking_flush() const
{
    m_commonFileSink->flush();
}
