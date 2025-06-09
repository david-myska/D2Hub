extends Control

func _ready() -> void:
	%AutoConnect.button_pressed = App.Config.get_value(Cfg.sec_global, Cfg.key_auto_attach)
	%SkipThis.button_pressed = App.Config.get_value(Cfg.sec_global, Cfg.key_skip_warning)
	%MXLDirLineEdit.text = App.Config.get_value(Cfg.sec_global, Cfg.key_mxl_dir)
	$DirDialog.dir_selected.connect(func(dir_path):
		%MXLDirLineEdit.text = dir_path
	)

func _on_auto_connect_toggled(toggled_on: bool) -> void:
	App.Config.set_value(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)


func _on_skip_this_toggled(toggled_on: bool) -> void:
	App.Config.set_value(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)


func _on_open_d2hub_pressed() -> void:
	get_tree().change_scene_to_file("res://main_scene.tscn")


func _on_open_dir_dialog_btn_pressed() -> void:
	$DirDialog.popup_centered()
