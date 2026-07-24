extends MarginContainer

func fill_conditions(achievement : Achievement):
	var conditions = preload("res://modules/achievements/conditions_view.tscn").instantiate()
	$VBoxContainer.add_child(conditions)
	conditions.from_achievement(achievement)

func from_achievement(achievement : Achievement):
	for c in $VBoxContainer.get_children():
		c.queue_free()
	var m := achievement.get_metadata()
	var name_lbl := Label.new()
	var description_lbl := Label.new()
	name_lbl.text = m["name"]
	description_lbl.text = m["description"]
	$VBoxContainer.add_child(name_lbl)
	$VBoxContainer.add_child(description_lbl)
	fill_conditions(achievement)
