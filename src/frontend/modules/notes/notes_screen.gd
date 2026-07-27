extends MarginContainer

func create_overlay_content():
	var content := preload("res://modules/notes/notes_overlay.tscn").instantiate()
	return content

func _ready() -> void:
	%GuideSelectBtn.add_item("None")
	for g in Backend.get_notes_module().get_available_guides():
		%GuideSelectBtn.add_item(g)
	%GuideSelectBtn.selected = 0


func _on_guide_select_btn_item_selected(index: int) -> void:
	if index == 0:
		Backend.get_notes_module().clear()
	else:
		Backend.get_notes_module().load_guide(%GuideSelectBtn.get_item_text(index))
	var guide_metadata := Backend.get_notes_module().get_current_guide_metadata()
	%GuideNameLbl.text = guide_metadata["name"]
	%GuideDescLbl.text = guide_metadata["description"]
