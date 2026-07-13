extends "res://modules/module_page.gd"

func _enter_tree() -> void:
	%ModuleStatus.m_module = Backend.get_notes_module()

func fill_overlay_panels(adder : Callable):
	adder.call("Achievements", $AchievementScreen.create_overlay_content())
