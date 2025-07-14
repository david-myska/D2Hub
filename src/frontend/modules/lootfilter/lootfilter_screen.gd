extends MarginContainer

func create_overlay_content():
	var content := preload("res://modules/lootfilter/lootfilter_overlay.tscn").instantiate()
	
	return content
