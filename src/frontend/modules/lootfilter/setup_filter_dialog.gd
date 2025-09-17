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
		af.set_selection(m_by_id_stat_data[f["id"]], f["op"], f["value"])

func _fill_category_filters(sf : Dictionary):
	pass

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
	d["type"] = MetaFilter.FilterType.SPECIAL
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
		"category_filters": {"predicate": 0, "filters": []},
		"special_filters": {"predicate": 0, "filters": []},
	}
	filters["special_filters"]["filters"].append(_make_quality_filter())
	for f in %AttributeFilters.get_children():
		var s : Dictionary = f.get_selection()
		s.merge(m_by_name_stat_data[s["stat_name"]])
		filters["stat_filters"]["filters"].append(s)
	if m_modify_idx < 0:
		Backend.get_lootfilter_module().add_filter(metadata, filters)
	else:
		Backend.get_lootfilter_module().modify_filter(m_modify_idx, metadata, filters)
	hide()
