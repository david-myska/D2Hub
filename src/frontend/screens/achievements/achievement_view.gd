extends Control

signal clicked()

func from_achievement(achievement):
	var m = achievement.get_metadata()
	%Title.text = m.get_name()
	%Description.text = m.get_description()
	achievement.status_changed.connect(_recolor)
	#_recolor(achievement.get_status())# TODO

func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		clicked.emit()

func _recolor(status : Achievement.Status) -> void:
	match status:
		Achievement.Status.DISABLED:
			$Background.color = Color.BLACK
		Achievement.Status.INACTIVE:
			$Background.color = Color.DIM_GRAY
		Achievement.Status.ACTIVE:
			$Background.color = Color.ROYAL_BLUE
		Achievement.Status.COMPLETED:
			$Background.color = Color.DARK_GOLDENROD
		Achievement.Status.FAILED:
			$Background.color = Color.DARK_RED
