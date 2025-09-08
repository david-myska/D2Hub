extends AcceptDialog

var m_stat_data : Dictionary = {}
var m_modify : bool = false

func _ready() -> void:
	m_stat_data = Backend.get_lootfilter_module().get_filter_categories()
	$AutoCompleteAssistant.load_terms(m_stat_data.keys())
	visibility_changed.connect(self.reset)

func reset():
	if visible:
		return
	for c in %AttributeFilters.get_children():
		c.remove_safely()
	
	for q in %Qualities.get_children():
		q.button_pressed = false
	
	%FilterName.clear()
	%NameLabel.modulate = Color.WHITE
	m_modify = false

func open_for_modify(filter : MetaFilter):
	m_modify = true

func _on_add_filter_btn_pressed() -> void:
	var f = preload("res://modules/lootfilter/attribute_filter.tscn").instantiate()
	f.m_autocomplete = $AutoCompleteAssistant
	%AttributeFilters.add_child(f)

func _make_quality_filter():
	var d := {}
	d["stat_id"] = 0
	d["stat_type"] = MetaFilter.FilterType.OTHER
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
		if not af.validate(m_stat_data):
			valid = false
	if not valid:
		return
	var metadata := FilterMetadata.new()
	metadata.name = %FilterName.text
	var filters := [_make_quality_filter()]
	for f in %AttributeFilters.get_children():
		var s : Dictionary = f.get_selection()
		s.merge(m_stat_data[s["stat_name"]])
		filters.append(s)
	Backend.get_lootfilter_module().add_filter(metadata, filters)
	hide()
