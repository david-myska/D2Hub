extends MarginContainer

var m_dev : DeveloperModule = Backend.get_developer_module()

var m_locked_pos : Vector2i = Vector2i.ZERO
var m_rel_pos : Vector2i = Vector2i.ZERO
var m_rel_pos2 : Vector2i = Vector2i.ZERO

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
	$FixStatDialog.validated.connect(func(stat_id : int, stat_name : String):
		m_dev.save_custom_stat(stat_id, stat_name)
	)
	$FixItemDialog.validated.connect(
		func(item_class : int, item_name : String, tier : int, fix_other_tiers : bool):
			if tier == 0: # Doesn't have tier -> Ring, Amulet, etc.
				m_dev.save_custom_item(item_class, item_name)
				return
			if not fix_other_tiers:
				m_dev.save_custom_item(item_class, "%s %s" % [item_name, tier_to_string(tier)])
				return
			var zeroed_tier : int = item_class - tier # Not having +1 here on purpose
			for i in range(1, 6):
				m_dev.save_custom_item(zeroed_tier + i, "%s %s" % [item_name, tier_to_string(i)])
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

func _refresh_item_stats(item : Dictionary):
	for c in %ItemStatsFlow.get_children():
		c.queue_free()
	for stat in item["stats"]:
		var fixable_stat := preload("res://modules/developer/fixable_stat.tscn").instantiate()
		fixable_stat.from_stat(stat)
		fixable_stat.stat_fix_requested.connect(open_fix_stat_dialog.bind(stat))
		%ItemStatsFlow.add_child(fixable_stat)

func open_fix_stat_dialog(stat : Dictionary):
	$FixStatDialog.show_filled(stat["id"], stat["name"])

func open_fix_item_dialog(item : Dictionary):
	$FixItemDialog.show_filled(item["item_class"], item["name"])

func _refresh_items(items : Array):
	for c in %ItemList.get_children():
		c.queue_free()
	for item in items:
		var fixable_item := preload("res://modules/developer/fixable_item.tscn").instantiate()
		fixable_item.from_item(item)
		fixable_item.item_fix_requested.connect(open_fix_item_dialog.bind(item))
		fixable_item.load_stats_requested.connect(_refresh_item_stats.bind(item))
		%ItemList.add_child(fixable_item)

var m_last_pressed_fnc : Callable

func _on_load_item_in_hand_btn_pressed() -> void:
	var loaded_item := m_dev.get_item_in_hand()
	if loaded_item.is_empty():
		return
	_refresh_items([loaded_item])
	m_last_pressed_fnc = _on_load_item_in_hand_btn_pressed


func _on_load_ground_items_btn_pressed() -> void:
	_refresh_items(m_dev.get_items_from(5)) # 5 == Dropped
	m_last_pressed_fnc = _on_load_ground_items_btn_pressed


func _on_load_vendor_items_btn_pressed() -> void:
	_refresh_items(m_dev.get_items_from(8)) # 8 == Vendor
	m_last_pressed_fnc = _on_load_vendor_items_btn_pressed


func _on_load_gamble_items_btn_pressed() -> void:
	_refresh_items(m_dev.get_items_from(9)) # 9 == Gamble
	m_last_pressed_fnc = _on_load_gamble_items_btn_pressed


func _on_load_inventory_items_btn_pressed() -> void:
	_refresh_items(m_dev.get_items_from(1)) # 1 == Inventory
	m_last_pressed_fnc = _on_load_inventory_items_btn_pressed


func _on_auto_refresh_timer_timeout() -> void:
	if m_last_pressed_fnc:
		m_last_pressed_fnc.call()


func _on_auto_refresh_btn_toggled(toggled_on: bool) -> void:
	if toggled_on:
		%AutoRefreshTimer.start()
	else:
		%AutoRefreshTimer.stop()


func _on_zone_id_btn_pressed() -> void:
	$HBoxContainer/GridContainer/ZoneId.text = str(m_dev.get_location_id())
