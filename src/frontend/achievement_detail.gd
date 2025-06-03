extends MarginContainer

func from_achievement(achievement):
	$Background/MarginContainer/VBoxContainer/AchievementView.from_achievement(achievement)
	print("details::from_achievement")
