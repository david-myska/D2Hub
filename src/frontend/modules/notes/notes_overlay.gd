extends VBoxContainer

func _ready() -> void:
	Backend.get_notes_module().notes_changed.connect(update_notes)
	
func update_notes():
	for c in get_children():
		c.queue_free()
	
	for group in Backend.get_notes_module().get_visible_notes():
		_add_group(group)

func _add_group(group : Dictionary):
	var name_lbl := Label.new()
	name_lbl.text = group["name"]
	add_child(name_lbl)
	for entry in group["entries"]:
		_add_group_entry(entry)

func _add_group_entry(entry : Dictionary):
	if entry.has("is_checked"):
		var checkbox := CheckBox.new()
		checkbox.text = entry["text"]
		checkbox.button_pressed = entry["is_checked"]
		add_child(checkbox)
	else:
		var lbl := Label.new()
		lbl.text = "  %s" % entry["text"]
		add_child(lbl)
