extends MarginContainer

func create_overlay_content():
	var content := preload("res://modules/notes/notes_overlay.tscn").instantiate()
	return content
