extends MarginContainer

func create_overlay_content():
	return preload("res://modules/statistics/statistics_overlay.tscn").instantiate()

func create_damage_meter_overlay_content():
	return preload("res://modules/statistics/damage_meter_overlay.tscn").instantiate()


func _on_reset_stats_btn_pressed() -> void:
	Backend.get_statistics_module().reset()


func _on_reset_dmg_btn_pressed() -> void:
	Backend.get_statistics_module().reset_dmg_stats()
