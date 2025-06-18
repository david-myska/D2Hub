extends Control

@onready var discover_timer := $DiscoverTimer

func _ready() -> void:
	var cmd_args := OS.get_cmdline_args()
	if "--developer" in cmd_args:
		var dev_screen = preload("res://screens/developer/developer_screen.tscn").instantiate()
		$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer.add_child(dev_screen)
		# TODO disable achievements when in developer mode
	
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
	var mxl_dir : String = App.Config.get_value(Cfg.sec_global, Cfg.key_mxl_dir)
	if Backend.is_mxl_dir_valid(mxl_dir):
		Backend.initialize_backend(App.Config.get_value(Cfg.sec_global, Cfg.key_mxl_dir))
	else:
		pass # TODO show some error in header status
	
	var auto_attach : bool = App.Config.get_value(Cfg.sec_global, Cfg.key_auto_attach)
	if auto_attach:
		%ManualStart.hide()
		Backend.start_memory_processor()


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
	for backup in Backend.get_available_backups():
		%BackupOption.add_item(backup)


func _on_load_backup_btn_pressed() -> void:
	Backend.recover_from_backup(%BackupOption.get_item_text(%BackupOption.selected))


func _on_create_backup_btn_pressed() -> void:
	Backend.manual_backup(%BackupNameLineEdit.text)
