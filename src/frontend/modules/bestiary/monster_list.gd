extends GridContainer

func fill(monsters : Dictionary):
	for c in get_children():
		c.queue_free()
	for m in monsters:
		add_entry(_preprocess_monsters(monsters[m]))

func _preprocess_monsters(named_monsters : Dictionary) -> Dictionary:
	var result := {}
	var mon = named_monsters.values()[0]
	for k in mon:
		result[k] = mon[k]
	result["count"] = named_monsters.size()
	return result

func add_entry(d : Dictionary):
	var name_lbl := Label.new()
	name_lbl.text = "%s (%s)" % [d["name"], d["count"]]
	add_child(name_lbl)
	var level_lbl := Label.new()
	level_lbl.text = str(d["level"])
	add_child(level_lbl)
	var max_life_lbl := Label.new()
	max_life_lbl.text = str(d["max_life"])
	add_child(max_life_lbl)
	var def_stats = _make_defense_stats(d)
	add_child(def_stats)

func _make_defense_stats(d : Dictionary):
	var def_stats := preload("res://modules/bestiary/defense_stats.tscn").instantiate()
	def_stats.from_monster(d)
	return def_stats
