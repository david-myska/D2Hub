extends "res://modules/module_page.gd"

func _enter_tree() -> void:
	%ModuleStatus.m_module = Backend.get_developer_module()
