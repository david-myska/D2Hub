extends VBoxContainer

func _ready() -> void:
	Backend.get_notes_module().notes_changed.connect(update_notes)
	
func update_notes():
	for c in get_children():
		c.queue_free()
	
	for note in Backend.get_notes_module().get_visible_notes():
		_add_note(note)

func _add_note(note : Dictionary):
	var lbl := Label.new()
	lbl.text = "%s%s" % [" ".repeat(note["indent"]), note["text"]]
	add_child(lbl)
