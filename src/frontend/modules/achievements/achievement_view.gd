extends Control

var m_achis := []
var m_collective_status : Achievement.Status = Achievement.Status.INACTIVE

func add_subachievement(achievement : Achievement, detail_opener : Callable):
	if m_achis.is_empty():
		%Title.text = achievement.get_metadata()["name"]
	m_achis.append(achievement)
	var subachi := preload("res://modules/achievements/subachievement_icon.tscn").instantiate()
	subachi.from_achievement(achievement)
	subachi.clicked.connect(detail_opener)
	achievement.status_changed.connect(_recolor) # eh
	_recolor(achievement.get_status())
	%SubAchiIcons.add_child(subachi)

func _ready() -> void:
	_recolor(m_collective_status)
	%SubAchiIcons.add_spacer(true)
	%SubAchiIcons.add_spacer(false)

func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		%SubAchiIcons.get_children().back().grab_click_focus()

func _recolor(_status : Achievement.Status) -> void:
	if m_achis.any(func(a): return a.get_status() == Achievement.Status.ACTIVE):
		m_collective_status = Achievement.Status.ACTIVE
	elif m_achis.any(func(a): return a.get_status() == Achievement.Status.FAILED):
		m_collective_status = Achievement.Status.FAILED
	elif m_achis.all(func(a): return a.get_status() == Achievement.Status.COMPLETED):
		m_collective_status = Achievement.Status.COMPLETED
	else:
		m_collective_status = Achievement.Status.INACTIVE
		
	match m_collective_status:
		Achievement.Status.ACTIVE: %Title.modulate = Color.ROYAL_BLUE
		Achievement.Status.COMPLETED: %Title.modulate = Color.DARK_GOLDENROD
		Achievement.Status.FAILED: %Title.modulate = Color.DARK_RED
		_: %Title.modulate = Color.WHITE
