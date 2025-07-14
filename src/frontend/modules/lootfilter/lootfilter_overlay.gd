extends Control

func _ready() -> void:
	Backend.get_lootfilter_module().loot_changed.connect(update_items)

func update_items():
	for c in get_children():
		c.queue_free()
	
	for d in Backend.get_lootfilter_module().get_passing_loot():
		var lbl := Label.new()
		lbl.text = d["location"] + ", " + str(d["position"])
		add_child(lbl)
