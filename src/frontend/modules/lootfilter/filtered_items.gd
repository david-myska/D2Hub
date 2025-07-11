extends MarginContainer

func _ready() -> void:
	Backend.get_lootfilter_module().loot_changed.connect(update_items)
	Backend.get_lootfilter_module().new_loot_notification.connect(func(path):
		print("New item notification: ", path)
	)

func update_items():
	for c in %ItemList.get_children():
		c.queue_free()
	
	for d in Backend.get_lootfilter_module().get_passing_loot():
		var lbl := Label.new()
		lbl.text = d["location"] + ", " + str(d["position"])
		%ItemList.add_child(lbl)
