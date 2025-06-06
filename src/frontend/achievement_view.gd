extends Control

signal clicked()

func from_achievement(achievement):
	var m = achievement.get_metadata()
	%Title.text = m.get_name()
	%Description.text = m.get_description()

func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		clicked.emit()
