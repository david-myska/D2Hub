extends MarginContainer

@onready var m_achis: VBoxContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiView/VBoxContainer
@onready var m_details: MarginContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiDetail/AchievementDetail

func _ready() -> void:
	fill_achievements()
	Backend.new_achievements_loaded.connect(fill_achievements)

func status_to_str(s : Achievement.Status) -> String:
	match s:
		Achievement.Status.DISABLED: return "Disabled"
		Achievement.Status.INACTIVE: return "Inactive"
		Achievement.Status.ACTIVE: return "Active"
		Achievement.Status.COMPLETED: return "Completed"
		Achievement.Status.FAILED: return "Failed"
	return "Unknown"

func _report_status(status : Achievement.Status, achi : Achievement):
	print("%s - Changed status to -> %s" % [achi.get_metadata().name, status_to_str(status)])

func fill_achievements() -> void:
	var achis := Backend.get_achievements()
	for c in m_achis.get_children():
		c.queue_free()
	for a in achis:
		var achi_view = preload("res://screens/achievements/achievement_view.tscn").instantiate()
		achi_view.from_achievement(a)
		m_achis.add_child(achi_view)
		achi_view.clicked.connect(m_details.from_achievement.bind(a))
		a.status_changed.connect(_report_status.bind(a))
