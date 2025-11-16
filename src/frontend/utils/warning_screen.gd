extends Control

func _ready() -> void:
	%SkipThis.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_skip_warning)

func _on_skip_this_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)

func _on_open_d2hub_pressed() -> void:
	get_tree().change_scene_to_file("res://main_scene.tscn")
