extends Control


func _on_timer_timeout() -> void:
	if App.Config.Get(Cfg.sec_global, Cfg.key_skip_warning):
		get_tree().change_scene_to_file("res://main_scene.tscn")
	else:
		get_tree().change_scene_to_file("res://utils/warning_screen.tscn")
