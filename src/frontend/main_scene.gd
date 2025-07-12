extends Control

@onready var discover_timer := $DiscoverTimer

func _ready() -> void:
	var cmd_args := OS.get_cmdline_args()
	if "--developer" in cmd_args:
		var dev_screen = preload("res://screens/developer/developer_screen.tscn").instantiate()
		$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer.add_child(dev_screen)
		# TODO disable achievements when in developer mode
	Backend.show_popup.connect($MessagePopup.show_message)
	Backend.target_process_exists.connect(func(exists : bool):
		%D2Discovered.text = "ON" if exists else "OFF"
		%D2Discovered.modulate = Color.GREEN if exists else Color.RED
		if exists:
			discover_timer.stop()
		else:
			discover_timer.start()
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
	
	var auto_attach : bool = App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach)
	if auto_attach:
		%ManualStart.hide()
		Backend.start_auto_attach()
	
	App.Config.changed.connect(func(sec : String, key : String):
		if sec == Cfg.sec_global and key == Cfg.key_auto_attach:
			if App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach):
				%ManualStart.hide()
				Backend.start_auto_attach()
			else:
				%ManualStart.show()
				Backend.stop_auto_attach()
	)
	
	var auto_backup : bool = App.Config.Get(Cfg.sec_backup, Cfg.key_auto_backup)
	if auto_backup:
		Backend.get_backup_module().enable_auto_backup(true)
	
	App.Config.changed.connect(func(sec : String, key : String):
		if sec == Cfg.sec_backup and key == Cfg.key_auto_backup:
			Backend.get_backup_module().enable_auto_backup(
				App.Config.Get(Cfg.sec_backup, Cfg.key_auto_backup))
	)
	
	#TODO TMP
	var overlay_id = $Overlay.get_window_id()
	var hwnd = DisplayServer.window_get_native_handle(DisplayServer.WINDOW_HANDLE, overlay_id)
	print("Overlay HWND:", hwnd)
	Backend.setup_overlay(hwnd)
	$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer/Settings.fix_overlay.connect(func(r : Rect2i):
		$Overlay.position = r.position
		$Overlay.size = r.size
	)


func _on_discover_timer_timeout() -> void:
	Backend.discover_target_process()


func _on_manual_start_pressed() -> void:
	Backend.start_memory_processor()


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
	print(%BackupOption.get_item_text(%BackupOption.selected))
	Backend.get_backup_module().recover_from_backup(%BackupOption.get_item_text(%BackupOption.selected))


func _on_create_backup_btn_pressed() -> void:
	Backend.get_backup_module().manual_backup(%BackupNameLineEdit.text)
