extends HBoxContainer

signal delete_requested()

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
	}

func _on_delete_btn_pressed() -> void:
	delete_requested.emit()
