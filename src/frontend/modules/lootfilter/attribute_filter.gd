extends HBoxContainer

signal delete_requested()

var m_categories : Dictionary

func _ready() -> void:
	%Categories.item_selected.connect(update_attributes)

func populate(categories : Dictionary, min_val : int = -1000000, max_val : int = 1000000):
	m_categories = categories
	
	%Categories.clear()
	for c in m_categories:
		%Categories.add_item(c, m_categories[c]["stat_type"])
	%Categories.select(0)
	update_attributes(0)
	
	%Comparators.clear()
	for c in ["=","≠", "<", "≤", ">", "≥", "has"]:
		%Comparators.add_item(c)
	%Comparators.selected = 0
	
	%Value.min_value = min_val
	%Value.max_value = max_val
	
	%Comparators.item_selected.connect(func(i):
		%Value.visible = i != 6
	)

func update_attributes(cat_idx : int):
	%Attributes.clear()
	for a in m_categories[%Categories.get_item_text(cat_idx)]["stats"]:
		#%Attributes.add_item(a["name"], a["stat_id"]) TODO
		%Attributes.add_item(a)
	%Attributes.selected = 0

func get_selection():
	return {
		"stat_type": %Categories.get_selected_id(),
		"stat_id": %Attributes.get_selected_id(),
		"op": %Comparators.get_selected_id(),
		"value": %Value.value,
	}

func _on_delete_btn_pressed() -> void:
	delete_requested.emit()
