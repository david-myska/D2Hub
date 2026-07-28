extends AcceptDialog

var m_by_name_stat_data : Dictionary = {}
var m_by_id_stat_data : Dictionary = {}
var m_modify_idx : int = -1

func _ready() -> void:
	var stats = Backend.get_lootfilter_module().get_stat_filter_categories()
	m_by_name_stat_data = stats["by_name"]
	m_by_id_stat_data = stats["by_id"]
	$AutoCompleteAssistant.load_terms(m_by_name_stat_data.keys())
	visibility_changed.connect(self.reset)
	hide()

func reset():
	if visible:
		return
	for c in %AttributeFilters.get_children():
		c.remove_safely()
	
	for q in %Qualities.get_children():
		q.button_pressed = false
	
	%FilterName.clear()
	%NameLabel.modulate = Color.WHITE
	m_modify_idx = -1

func open_for_add():
	ok_button_text = "Create"
	show()

func open_for_modify(index : int, filter : MetaFilter):
	m_modify_idx = index
	ok_button_text = "Modify"
	%FilterName.text = filter.get_metadata().name
	show()
	# Weird bug with Autocomplete shit, would crash if _fill_attr* called before showing
	await get_tree().create_timer(0.05).timeout
	_fill_attribute_filters(filter.get_stat_filters())
	_fill_category_filters(filter.get_category_filters())
	_fill_special_filters(filter.get_special_filters())

func _on_add_filter_btn_pressed() -> void:
	var f = preload("res://modules/lootfilter/attribute_filter.tscn").instantiate()
	f.m_autocomplete = $AutoCompleteAssistant
	%AttributeFilters.add_child(f)

func _fill_attribute_filters(sf : Dictionary):
	%StatGroupPredicate.selected = sf["predicate"]
	for f in sf["filters"]:
		var af = preload("res://modules/lootfilter/attribute_filter.tscn").instantiate()
		af.m_autocomplete = $AutoCompleteAssistant
		%AttributeFilters.add_child(af)
		af.set_selection(m_by_id_stat_data[f["id"]], f["op"], f["value"], f["compare_with_equipped"])

func _fill_category_filters(cf : Dictionary):
	var tier_filters = cf["filters"][0]
	if !tier_filters.is_empty():
		for tf in tier_filters["filters"]:
			%Tiers.get_child(tf["value"]).button_pressed = true
	
	var slot_filters = cf["filters"][1]
	if !slot_filters.is_empty():
		for sf in slot_filters["filters"]:
			%Slots.get_child(sf["value"]).button_pressed = true

func _fill_special_filters(sf : Dictionary):
	for f in sf["filters"]:
		match f["id"]:
			0: _fill_quality(f["value"])
			_: pass # TODO log

func _fill_quality(q : int):
	%Qualities/Normal.button_pressed = q & MetaFilter.Quality.NORMAL
	%Qualities/Superior.button_pressed = q & MetaFilter.Quality.SUPERIOR
	%Qualities/Magic.button_pressed = q & MetaFilter.Quality.MAGIC
	%Qualities/Rare.button_pressed = q & MetaFilter.Quality.RARE
	%Qualities/Set.button_pressed = q & MetaFilter.Quality.SET
	%Qualities/Unique.button_pressed = q & MetaFilter.Quality.UNIQUE

func _make_quality_filter():
	var d := {}
	d["id"] = 0
	d["op"] = 0 # Necessary to fulfill contract
	d["value"] = 0
	if %Qualities/Normal.button_pressed:
		d["value"] += MetaFilter.Quality.NORMAL
	if %Qualities/Superior.button_pressed:
		d["value"] += MetaFilter.Quality.SUPERIOR
	if %Qualities/Magic.button_pressed:
		d["value"] += MetaFilter.Quality.MAGIC
	if %Qualities/Rare.button_pressed:
		d["value"] += MetaFilter.Quality.RARE
	if %Qualities/Set.button_pressed:
		d["value"] += MetaFilter.Quality.SET
	if %Qualities/Unique.button_pressed:
		d["value"] += MetaFilter.Quality.UNIQUE
	
	if d["value"] == 0:# Nothing selected, allow all
		d["value"] += MetaFilter.Quality.NORMAL\
					+ MetaFilter.Quality.SUPERIOR\
					+ MetaFilter.Quality.MAGIC\
					+ MetaFilter.Quality.RARE\
					+ MetaFilter.Quality.SET\
					+ MetaFilter.Quality.UNIQUE
	return d

func _make_tier_subfilter(tier : MetaFilter.Tier):
	return {
		"id": 0,
		"value": tier,
		"op": 0, # Necessary to fulfill contract
	}

func _make_tier_filter():
	var d := {}
	d["predicate"] = MetaFilter.Predicate.ANY
	d["filters"] = []
	
	if %Tiers/NoTier.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.NO_TIER))
	if %Tiers/Tier1.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.TIER_1))
	if %Tiers/Tier2.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.TIER_2))
	if %Tiers/Tier3.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.TIER_3))
	if %Tiers/Tier4.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.TIER_4))
	if %Tiers/Sacred.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.SACRED))
	if %Tiers/Angelic.button_pressed:
		d["filters"].append(_make_tier_subfilter(MetaFilter.Tier.ANGELIC))
	
	if d["filters"].is_empty():# Nothing selected, allow all
		# filters == [] + predicate == ALL -> tautology
		d["predicate"] = MetaFilter.Predicate.ALL 
	return d

func _make_slot_subfilter(slot : MetaFilter.Slot):
	return {
		"id": 1,
		"value": slot,
		"op": 0, # Necessary to fulfill contract
	}

func _make_slot_filter():
	var d := {}
	d["predicate"] = MetaFilter.Predicate.ANY
	d["filters"] = []
	
	if %Slots/None.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.NONE))
	if %Slots/Helm.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.HELM))
	if %Slots/Amulet.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.AMULET))
	if %Slots/BodyArmor.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.BODY_ARMOR))
	if %Slots/Weapon.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.MAIN_HAND))
	if %Slots/Shield.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.OFF_HAND))
	if %Slots/Ring.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.RING))
	if %Slots/Belt.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.BELT))
	if %Slots/Boots.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.BOOTS))
	if %Slots/Gloves.button_pressed:
		d["filters"].append(_make_slot_subfilter(MetaFilter.Slot.GLOVES))
	
	if d["filters"].is_empty():# Nothing selected, allow all
		# filters == [] + predicate == ALL -> tautology
		d["predicate"] = MetaFilter.Predicate.ALL
	return d

func _on_confirmed() -> void:
	var valid := true
	if %FilterName.text.is_empty():
		%NameLabel.modulate = Color.RED
		valid = false
	else:
		%NameLabel.modulate = Color.WHITE
	for af in %AttributeFilters.get_children():
		if not af.validate(m_by_name_stat_data):
			valid = false
	if not valid:
		return
	var metadata := FilterMetadata.new()
	metadata.name = %FilterName.text
	var filters := {
		"stat_filters": {"predicate": %StatGroupPredicate.selected, "filters": []},
		"category_filters": {"predicate": MetaFilter.Predicate.ALL, "filters": []},
		"special_filters": {"predicate": MetaFilter.Predicate.ALL, "filters": []},
	}
	filters["special_filters"]["filters"].append(_make_quality_filter())
	filters["category_filters"]["filters"].append(_make_tier_filter())
	filters["category_filters"]["filters"].append(_make_slot_filter())
	for f in %AttributeFilters.get_children():
		var s : Dictionary = f.get_selection()
		s.merge(m_by_name_stat_data[s["stat_name"]])
		filters["stat_filters"]["filters"].append(s)
	if m_modify_idx < 0:
		Backend.get_lootfilter_module().add_filter(metadata, filters)
	else:
		Backend.get_lootfilter_module().modify_filter(m_modify_idx, metadata, filters)
	hide()
