extends Control

signal clicked()

var m_achi : Achievement = null

func from_achievement(achievement : Achievement):
	m_achi = achievement
	var m = m_achi.get_metadata()
	%Title.text = m["name"]
	%Description.text = m["description"]
	m_achi.status_changed.connect(_recolor)
	_recolor(m_achi.get_status())

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
