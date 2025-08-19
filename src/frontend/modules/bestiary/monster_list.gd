extends GridContainer

func fill(monsters : Array):
	for c in get_children():
		c.queue_free()
	for m in monsters:
		add_entry(m)

func add_entry(d : Dictionary):
	var name_lbl := Label.new()
	name_lbl.text = d["name"]
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
