extends HBoxContainer

var m_autocomplete : AutoCompleteAssistant = null

func _ready() -> void:
	if m_autocomplete:
		m_autocomplete.add_edit(%Attribute)
	for c in ["=","≠", "<", "≤", ">", "≥", "has"]:
		%Comparators.add_item(c)
	%Comparators.selected = 0
	%Comparators.item_selected.connect(func(i):
		%Value.visible = i != 6
	)

func get_selection():
	return {
		"stat_name": %Attribute.text,
		"op": %Comparators.get_selected_id(),
		"value": %Value.value,
		"compare_with_equipped": %CompareWithEquipped.button_pressed
	}

func set_selection(stat_name : String, op : int, value : float, compare_with_equipped : bool):
	%Attribute.text = stat_name
	%Comparators.selected = op
	%Value.value = value
	%CompareWithEquipped.button_pressed = compare_with_equipped

func remove_safely():
	if m_autocomplete and %Attribute:
		m_autocomplete.remove_edit(%Attribute)
	queue_free()

func _on_delete_btn_pressed() -> void:
	remove_safely()

func validate(d : Dictionary) -> bool:
	if %Attribute.text in d:
		%Attribute.modulate = Color.WHITE
		return true
	%Attribute.modulate = Color.RED
	return false
