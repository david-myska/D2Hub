extends Control

func _ready() -> void:
	%AutoConnect.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach)
	%SkipThis.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_skip_warning)
	$DirDialog.dir_selected.connect(func(dir_path):
		%SaveDirLineEdit.text = dir_path
		Backend.get_backup_module().initialize(dir_path)
	)
	%SaveDirLineEdit.text = App.Config.Get(Cfg.sec_backup, Cfg.key_saves_dir)

func _on_auto_connect_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)


func _on_skip_this_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)


func _on_open_d2hub_pressed() -> void:
	get_tree().change_scene_to_file("res://main_scene.tscn")


func _on_open_save_dir_dialog_btn_pressed() -> void:
	$DirDialog.popup_centered()
