extends HBoxContainer

signal delete_request()

func populate(attributes : Array, min_val : int = -1000000, max_val : int = 1000000):
	%Attributes.clear()
	for a in attributes:
		%Attributes.add_item(a)
	%Attributes.selected = 0
	
	%Comparators.clear()
	for c in ["=","≠", "<", "≤", ">", "≥", "has"]:
		%Comparators.add_item(c)
	%Comparators.selected = 0
	
	%Value.min_value = min_val
	%Value.max_value = max_val
	
	%Comparators.item_selected.connect(func(i):
		%Value.visible = i != 6
	)


func get_selection():
	return {
		"attr": %Attributes.get_selected_id(),
		"comp": %Comparators.get_selected_id(),
		"val": %Value.value,
	}

func _on_delete_btn_pressed() -> void:
	delete_request.emit()
