extends "res://modules/module_page.gd"

func _enter_tree() -> void:
	%ModuleStatus.m_module = Backend.get_statistics_module()

func fill_overlay_panels(adder : Callable):
	adder.call("Statistics", $StatisticsScreen.create_overlay_content())
	adder.call("DamageMeter", $StatisticsScreen.create_damage_meter_overlay_content())
