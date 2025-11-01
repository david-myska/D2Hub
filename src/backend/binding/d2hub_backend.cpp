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
        m_bestiaryModule->Update(*m_dataAccess, *m_sharedData);
        m_lootfilterModule->Update(*m_dataAccess, *m_sharedData);
        m_statisticsModule->Update(*m_dataAccess, *m_sharedData);
        m_developerModule->Update(*m_dataAccess, *m_sharedData);
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("Error during update: {}", e.what());
        m_notifier->Push(MessageType::Error, msg);
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

void D2HubBackend::StartMemoryProcessor()
{
    if (m_memoryProcessor->IsRunning())
    {
        m_logView->Log(*m_logger, "Memory processor is already running.", MessageType::Warning);
        return;
    }

    m_memoryProcessor->SetUpdateCallback(
        [this](const GE::DataAccessor&) {
            if (!CanUpdate())
            {
                return;
            }
            Update();
        },
        c_framesToKeep, 1000 / m_updatesPerSecond);

    if (m_targetProcess->IsAttached())
    {
        m_logView->Log(*m_logger, std::format("Starting memory processor: keep {} memory scans and scan {}x/second",
                                              c_framesToKeep, m_updatesPerSecond));
        m_memoryProcessor->RequestStart(m_targetProcess->GetMemoryAccess());
    }
    else
    {
        m_logView->Log(*m_logger, "Cannot start memory processor: TargetProcess is not attached.", MessageType::Warning);
    }
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
    ClassDB::bind_method(D_METHOD("set_update_rate", "updates_per_second"), &D2HubBackend::set_update_rate);
    ClassDB::bind_method(D_METHOD("get_update_rate"), &D2HubBackend::get_update_rate);

    ClassDB::bind_method(D_METHOD("enable_auto_attach", "enable"), &D2HubBackend::enable_auto_attach);

    ClassDB::bind_method(D_METHOD("start_auto_attach"), &D2HubBackend::start_auto_attach);
    ClassDB::bind_method(D_METHOD("stop_auto_attach"), &D2HubBackend::stop_auto_attach);
    ClassDB::bind_method(D_METHOD("discover_target_process"), &D2HubBackend::discover_target_process);
    ClassDB::bind_method(D_METHOD("target_process_exists"), &D2HubBackend::target_process_exists);
    ClassDB::bind_method(D_METHOD("attach_to_target_process", "p_attach"), &D2HubBackend::attach_to_target_process);
    ClassDB::bind_method(D_METHOD("start_memory_processor"), &D2HubBackend::start_memory_processor);
    ClassDB::bind_method(D_METHOD("stop_memory_processor"), &D2HubBackend::stop_memory_processor);

    ClassDB::bind_method(D_METHOD("get_notifier"), &D2HubBackend::get_notifier);

    ClassDB::bind_method(D_METHOD("get_achievements_module"), &D2HubBackend::get_achievements_module);
    ClassDB::bind_method(D_METHOD("get_backup_module"), &D2HubBackend::get_backup_module);
    ClassDB::bind_method(D_METHOD("get_bestiary_module"), &D2HubBackend::get_bestiary_module);
    ClassDB::bind_method(D_METHOD("get_developer_module"), &D2HubBackend::get_developer_module);
    ClassDB::bind_method(D_METHOD("get_lootfilter_module"), &D2HubBackend::get_lootfilter_module);
    ClassDB::bind_method(D_METHOD("get_statistics_module"), &D2HubBackend::get_statistics_module);
    ClassDB::bind_method(D_METHOD("get_modules"), &D2HubBackend::get_modules);

    ClassDB::bind_method(D_METHOD("get_target_rect"), &D2HubBackend::get_target_rect);
    ClassDB::bind_method(D_METHOD("enable_window_clickthrough", "p_hwnd", "p_enable"), &D2HubBackend::enable_window_clickthrough);
    ClassDB::bind_method(D_METHOD("is_target_window_focused"), &D2HubBackend::is_target_window_focused);

    ClassDB::bind_method(D_METHOD("fucking_flush"), &D2HubBackend::fucking_flush);
    ClassDB::bind_method(D_METHOD("send_unhandled_exception"), &D2HubBackend::send_unhandled_exception);

    ADD_SIGNAL(MethodInfo("target_process_existence_changed", PropertyInfo(Variant::BOOL, "exists")));
    ADD_SIGNAL(MethodInfo("target_process_attached", PropertyInfo(Variant::BOOL, "attached")));
    ADD_SIGNAL(MethodInfo("memory_processor_running", PropertyInfo(Variant::BOOL, "processing")));
}

D2HubBackend::D2HubBackend()
    : m_logLevel(ParseLogLevel())
    , m_commonFileSink(MakeLoggerSink())
    , m_logger(MakeLogger("d2hub_backend"))
    , m_logView(LogView::Create(MakeLogger("frontend")))
    , m_notifier(Notifier::Create(MakeLogger("notifier"), m_logView))
    , m_achievementsModule(AchievementsModule::Create(MakeLogger("achievements_module"), m_notifier, m_logView))
    , m_backupModule(BackupModule::Create(MakeLogger("backup_module"), m_notifier, m_logView))
    , m_bestiaryModule(BestiaryModule::Create(MakeLogger("bestiary_module"), m_notifier, m_logView))
    , m_developerModule(DeveloperModule::Create(MakeLogger("developer_module"), m_notifier, m_logView))
    , m_lootfilterModule(LootFilterModule::Create(MakeLogger("lootfilter_module"), m_notifier, m_logView))
    , m_statisticsModule(StatisticsModule::Create(MakeLogger("statistics_module"), m_notifier, m_logView))
    , m_modules(
          {m_achievementsModule, m_backupModule, m_bestiaryModule, m_developerModule, m_lootfilterModule, m_statisticsModule})
{
    D2::Data::LoadStats();
    D2::Data::LoadItems();
    D2::Data::LoadMinions();
    InitializeModuleDependencies();
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

Ref<BestiaryModule> D2HubBackend::get_bestiary_module()
{
    return m_bestiaryModule;
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

uint32_t D2HubBackend::get_update_rate()
{
    return m_updatesPerSecond;
}

void D2HubBackend::set_update_rate(uint32_t updates_per_second)
{
    if (updates_per_second == 0)
    {
        m_logView->Log(*m_logger, "Update rate cannot be zero.", MessageType::Warning);
        return;
    }
    m_updatesPerSecond = updates_per_second;
}

void D2HubBackend::enable_auto_attach(bool enable)
{
    if (enable)
    {
        m_startOnAttachedToken = m_targetProcess->OnAttachmentChanged([this](bool aAttached) {
            if (aAttached)
            {
                StartMemoryProcessor();
            }
            else
            {
                m_memoryProcessor->RequestStop();
            }
        });

        m_logView->Log(*m_logger, "Enabling auto-attach to target process");
        start_auto_attach();
    }
    else
    {
        m_logView->Log(*m_logger, "Disabling auto-attach to target process");
        m_startOnAttachedToken.reset();
        stop_auto_attach();
    }
}

void D2HubBackend::InitializeModuleDependencies()
{
    for (auto module : m_modules)
    {
        for (auto other_module : m_modules)
        {
            Object::cast_to<Module>(module)->SetIfDependency(Object::cast_to<Module>(other_module));
        }
    }
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
        call_deferred("emit_signal", "target_process_existence_changed", aTargetProcessExists);
    });
    m_targetProcessAttachmentToken = m_targetProcess->OnAttachmentChanged([this](bool aTargetProcessAttached) {
        call_deferred("emit_signal", "target_process_attached", aTargetProcessAttached);
    });
    m_memoryProcessor = GE::MemoryProcessor::Create(MakeLogger("memory_processor"));
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

bool D2HubBackend::target_process_exists()
{
    return m_targetProcess && m_targetProcess->Exists();
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
        StartMemoryProcessor();
    }
    catch (const std::exception& e)
    {
        m_logView->Log(*m_logger, std::format("Failed to start memory processor: {}", e.what()), MessageType::Error);
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
        m_logView->Log(*m_logger, std::format("Failed to stop memory processor: {}", e.what()), MessageType::Error);
    }
}

#include <windows.h>

constexpr auto c_tmpWindowName = "Diablo II";

Rect2i D2HubBackend::get_target_rect() const
{
    auto w = FindWindowA(nullptr, c_tmpWindowName);
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

bool D2HubBackend::is_target_window_focused()
{
    auto hwnd = FindWindowA(nullptr, c_tmpWindowName);
    return hwnd == GetForegroundWindow();
}

void D2HubBackend::fucking_flush() const
{
    m_commonFileSink->flush();
}

void D2HubBackend::send_unhandled_exception()
{
    std::thread thrower([]() {
        throw std::runtime_error("This is an unhandled exception from D2HubBackend");
    });
    thrower.join();
}
