extends MarginContainer

var m_dev : DeveloperModule = Backend.get_developer_module()

var m_locked_pos : Vector2i = Vector2i.ZERO
var m_rel_pos : Vector2i = Vector2i.ZERO
var m_rel_pos2 : Vector2i = Vector2i.ZERO

var m_loaded_item := {}

func tier_to_string(tier : int) -> String:
	match tier:
		0: return "(No tier)"
		1: return "(1)"
		2: return "(2)"
		3: return "(3)"
		4: return "(4)"
		5: return "(Sacred)"
	return "(Unknown tier %s)" % tier

func _ready():
	%LoadedItemStuff.visible = false
	$FixStatDialog.validated.connect(func(stat_id : int, stat_name : String, categories : String):
		m_dev.save_custom_stat(stat_id, stat_name, categories)
	)
	$FixItemDialog.validated.connect(
		func(item_class : int, item_name : String, tier : int, fix_other_tiers : bool):
			if tier == 0:
				m_dev.save_custom_item(item_class, item_name)
				return
			if not fix_other_tiers:
				m_dev.save_custom_item(item_class, "%s %s" % [item_name, tier_to_string(tier)])
				return
			var zero_tier_class : int = item_class - tier # Not having +1 here on purpose
			for i in range(1, 6):
				m_dev.save_custom_item(zero_tier_class + i, "%s %s" % [item_name, tier_to_string(tier)])
	)

func _on_lock_pos_btn_pressed() -> void:
	m_locked_pos = m_dev.get_player_position()
	$HBoxContainer/GridContainer/LockedPos.text = str(m_locked_pos)
	_update_rel(m_rel_pos)

func _update_rel(to_point : Vector2i):
	m_rel_pos = to_point
	$HBoxContainer/GridContainer/RelativePos.text = str(m_rel_pos - m_locked_pos)

func _on_relative_pos_btn_pressed() -> void:
	_update_rel(m_dev.get_player_position())
	_update_rel2(m_rel_pos2)

func _update_rel2(to_point : Vector2i):
	m_rel_pos2 = to_point
	$HBoxContainer/GridContainer/RelativePos2.text = str(m_rel_pos2 - m_rel_pos)

func _on_relative_pos_btn_2_pressed() -> void:
	_update_rel2(m_dev.get_player_position())


func _on_load_item_in_hand_btn_pressed() -> void:
	var loaded_item := m_dev.get_item_in_hand()
	if loaded_item.is_empty():
		Backend.get_notifier().push(Notifier.WARNING, "No item in hand")
		return
	m_loaded_item = loaded_item
	_refresh_loaded_item()

func _refresh_loaded_item():
	for c in %ItemStatsFlow:
		c.queue_free()
	%ItemName.text = m_loaded_item["name"]
	for stat in m_loaded_item["stats"]:
		var fixable_stat := preload("res://modules/developer/fixable_stat.tscn").instantiate()
		fixable_stat.from_stat(stat)
		fixable_stat.stat_fix_requested.connect(open_fix_stat_dialog.bind(stat))
		%ItemStatsFlow.add_child(fixable_stat)
	%LoadedItemStuff.visible = true

func open_fix_stat_dialog(stat : Dictionary):
	$FixStatDialog.show_filled(stat["id"], stat["name"], stat["categories"])

func open_fix_item_dialog(item : Dictionary):
	$FixItemDialog.show_filled(item["item_class"], item["name"])


func _on_fix_name_btn_pressed() -> void:
	open_fix_item_dialog(m_loaded_item)
