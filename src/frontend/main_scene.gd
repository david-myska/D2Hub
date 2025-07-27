extends Control

@onready var discover_timer := $DiscoverTimer

var m_overlay_hwnd : int = 0

func _ready() -> void:
	var cmd_args := OS.get_cmdline_args()
	if "--developer" in cmd_args:
		var dev_screen = preload("res://modules/developer/developer_screen.tscn").instantiate()
		dev_screen.name = "Developer"
		$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer.add_child(dev_screen)
		# TODO disable achievements when in developer mode
	Backend.target_process_exists.connect(func(exists : bool):
		%D2Discovered.text = "ON" if exists else "OFF"
		%D2Discovered.modulate = Color.GREEN if exists else Color.RED
		_disable_backup(exists)
	)
	Backend.target_process_attached.connect(func(attached : bool):
		%Attached.text = "ON" if attached else "OFF"
		%Attached.modulate = Color.GREEN if attached else Color.RED
	)
	Backend.memory_processor_running.connect(func(running : bool):
		%Processing.text = "ON" if running else "OFF"
		%Processing.modulate = Color.GREEN if running else Color.RED
	)
	
	if App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach):
		%ManualStart.hide()
		Backend.enable_auto_attach(true)
	
	Backend.get_backup_module().enable_auto_backup(App.Config.Get(Cfg.sec_backup, Cfg.key_auto_backup))
	App.Config.changed.connect(func(sec : String, key : String, v : Variant):
		if sec == Cfg.sec_backup and key == Cfg.key_auto_backup:
			Backend.get_backup_module().enable_auto_backup(v)
	)
	
	$Overlay.visible = App.Config.Get(Cfg.sec_overlay, Cfg.key_overlay_enabled)
	App.Config.changed.connect(func(sec : String, key : String, v : Variant):
		if sec == Cfg.sec_overlay and key == Cfg.key_overlay_enabled:
			$Overlay.enable(v)
	)
	
	%Settings.fix_overlay.connect($Overlay.fill)
	%Settings.overlay_edit_mode.connect($Overlay.enable_edit_mode)
	
	$Overlay.add_panel("Achievements", %Achievements.create_overlay_content())
	$Overlay.add_panel("Notifications", preload("res://utils/notifications/notification_overlay.tscn").instantiate())
	$Overlay.add_panel("LootFilter", %Lootfilter.create_overlay_content())


func _on_discover_timer_timeout() -> void:
	Backend.discover_target_process()
	$Overlay.fill(Backend.get_target_rect())
	Backend.fucking_flush()
	Backend.get_notifier().push(Notifier.WARNING, "Testing overlay: %s" % randi())


func _on_manual_start_pressed() -> void:
	if %ManualStart.text == "Start":
		Backend.start_memory_processor()
		%ManualStart.text = "Stop"
	else:
		Backend.stop_memory_processor()
		Backend.attach_to_target_process(false)
		%ManualStart.text = "Start"


func _on_show_logs_pressed() -> void:
	OS.shell_open(ProjectSettings.globalize_path("user://logs"))

func _disable_backup(disable : bool) -> void:
	$MarginContainer/VBoxContainer/Header/PanelContainer/HBoxContainer/CreateBackup/CreateBackupBtn.disabled = disable
	$MarginContainer/VBoxContainer/Header/PanelContainer/HBoxContainer/LoadBackup/LoadBackupBtn.disabled = disable

func _on_backup_option_pressed() -> void:
	%BackupOption.clear()
	for backup in Backend.get_backup_module().get_available_backups():
		%BackupOption.add_item(backup)


func _on_load_backup_btn_pressed() -> void:
	if %BackupOption.selected < 0:
		return
	Backend.get_backup_module().recover_from_backup(%BackupOption.get_item_text(%BackupOption.selected))


func _on_create_backup_btn_pressed() -> void:
	Backend.get_backup_module().manual_backup(%BackupNameLineEdit.text)
