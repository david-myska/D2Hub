extends GridContainer

@export var m_affinity : BestiaryModule.Affinity

@onready var m_change_string :=\
	("Monster" if m_affinity != BestiaryModule.MONSTERS else "Companion")
@onready var m_change_affinity :=\
	(BestiaryModule.MONSTERS if m_affinity != BestiaryModule.MONSTERS else BestiaryModule.COMPANIONS)

func fill(npcs : Dictionary):
	for c in get_children():
		c.queue_free()
	for n in npcs:
		add_entry(_preprocess_npcs(npcs[n]))

func _preprocess_npcs(named_npcs : Dictionary) -> Dictionary:
	var result := {}
	var npc = named_npcs.values()[0]
	for k in npc:
		result[k] = npc[k]
	result["count"] = named_npcs.size()
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
	var switch_btn := Button.new()
	switch_btn.text = "This is %s" % m_change_string
	switch_btn.pressed.connect(func():
		Backend.get_bestiary_module().change_npc_affinity(d["class"], m_change_affinity)
	)
	add_child(switch_btn)

func _make_defense_stats(d : Dictionary):
	var def_stats := preload("res://modules/bestiary/defense_stats.tscn").instantiate()
	def_stats.from_monster(d)
	return def_stats
