extends AcceptDialog

var m_populate_data : Dictionary = {}

func _ready() -> void:
	m_populate_data = Backend.get_lootfilter_module().get_filter_categories()
	visibility_changed.connect(self.reset)

func reset():
	if not visible:
		return
	for c in %AttributeFilters.get_children():
		c.queue_free()
	
	for q in %Qualities.get_children():
		q.button_pressed = false
	
	%FilterName.clear()
	%NameLabel.modulate = Color.WHITE


func _on_add_filter_btn_pressed() -> void:
	var f = preload("res://modules/lootfilter/attribute_filter.tscn").instantiate()
	f.populate(m_populate_data)
	f.delete_requested.connect(%AttributeFilters.remove_child.bind(f))
	%AttributeFilters.add_child(f)

func _make_quality_filter():
	var d := {}
	d["stat_id"] = 0
	d["stat_type"] = MetaFilter.StatType.OTHER
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
	if %FilterName.text.is_empty():
		%NameLabel.modulate = Color.RED
		return
	var metadata := FilterMetadata.new()
	metadata.name = %FilterName.text
	var filters := [_make_quality_filter()]
	for f in %AttributeFilters.get_children():
		filters.append(f.get_selection())
	Backend.get_lootfilter_module().add_filter(metadata, filters)
	hide()
