extends MarginContainer

@onready var m_conditions: MarginContainer = $VBoxContainer/ConditionsView

func to_str(c : Achievement.Condition) -> String:
	match c:
		Achievement.Condition.PRECONDITION:
			return "Preconditions"
		Achievement.Condition.ACTIVATOR:
			return "Activators"
		Achievement.Condition.COMPLETER:
			return "Completers"
		Achievement.Condition.FAILER:
			return "Failers"
		Achievement.Condition.VALIDATOR:
			return "Validators"
		Achievement.Condition.RESETER:
			return "Reseters"
	return "Invalid condition"

func fill_conditions(achievement : Achievement):
	m_conditions.clear()
	var by_category := achievement.get_conditions().get_conditions_by_categories()
	for c in range(Achievement.ALL_CONDITIONS):
		if by_category[c].is_empty():
			continue
		m_conditions.add_name(to_str(c))
		m_conditions.add_texts(by_category[c].values())
	achievement.progress_made.connect(m_conditions.wtf.bind(achievement))
	achievement.status_changed.connect(func(s : Achievement.Status):
		print("new status: %s" % s)
	)

func from_achievement(achievement : Achievement):
	$VBoxContainer/AchievementView.from_achievement(achievement)
	fill_conditions(achievement)
