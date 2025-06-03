extends MarginContainer

@onready var m_achis: VBoxContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiView/VBoxContainer
@onready var m_details: MarginContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiDetail/AchievementDetail

func fill_achievements(achis : Array) -> void:
	for c in m_achis.get_children():
		c.queue_free()
	for a in achis:
		var achi_view = preload("res://achievement_view.tscn").instantiate()
		achi_view.from_achievement(a)
		m_achis.add_child(achi_view)
		achi_view.clicked.connect(m_details.from_achievement.bind(a))
