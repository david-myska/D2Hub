extends MarginContainer

func fill_conditions(achievement : Achievement):
	var conditions = preload("res://screens/achievements/conditions_view.tscn").instantiate()
	$VBoxContainer.add_child(conditions)
	conditions.from_achievement(achievement)

func from_achievement(achievement : Achievement):
	for c in $VBoxContainer.get_children():
		c.queue_free()
	var achi_view = preload("res://screens/achievements/achievement_view.tscn").instantiate()
	achi_view.from_achievement(achievement)
	$VBoxContainer.add_child(achi_view)
	fill_conditions(achievement)
