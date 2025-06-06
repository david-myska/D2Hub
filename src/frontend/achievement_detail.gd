extends MarginContainer

@onready var m_conditions: MarginContainer = $VBoxContainer/ConditionsView


func fill_conditions(achievement : Achievement):
	m_conditions.clear()
	var by_category := achievement.get_conditions().get_conditions_by_categories()
	for c in range(Achievement.ALL_CONDITIONS):
		if by_category[c].is_empty():
			continue
		m_conditions.add_name(str(c))
		m_conditions.add_texts(by_category[c].values())
	achievement.progress_made.connect(m_conditions.wtf.bind(achievement))

func from_achievement(achievement : Achievement):
	$VBoxContainer/AchievementView.from_achievement(achievement)
	fill_conditions(achievement)
