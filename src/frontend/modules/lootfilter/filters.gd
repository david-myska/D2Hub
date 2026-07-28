extends MarginContainer

var m_filter_profile_duplicate : bool = false

func _ready() -> void:
	Backend.get_lootfilter_module().filters_changed.connect(_update_filters)
	_update_filters()
	
	%FilterProfileDialog.visible = false
	Backend.get_lootfilter_module().filter_profiles_changed.connect(_update_filter_profiles)
	_update_filter_profiles()

func _update_filters():
	for c in %FilterList.get_children():
		c.queue_free()
	
	var i := 0
	for mf in Backend.get_lootfilter_module().get_filters():
		var f = preload("res://modules/lootfilter/filter.tscn").instantiate()
		f.m_metadata = mf.get_metadata()
		f.m_metadata.notification_path = f.m_metadata.name# TMP for testing
		f.delete_requested.connect(Backend.get_lootfilter_module().remove_filter.bind(i))
		f.modify_requested.connect(%SetupFilterDialog.open_for_modify.bind(i, mf))
		f.duplicate_requested.connect(Backend.get_lootfilter_module().duplicate_filter.bind(i))
		%FilterList.add_child(f)
		i += 1

func _on_add_filter_pressed() -> void:
	%SetupFilterDialog.open_for_add()

func _update_filter_profiles():
	_fill_profile_selector()
	var p := Backend.get_lootfilter_module().get_selected_profile()
	for i in %ProfileSelector.item_count:
		if %ProfileSelector.get_item_text(i) == p:
			%ProfileSelector.select(i)
			return
	%ProfileSelector.select(0)

func _fill_profile_selector():
	%ProfileSelector.clear()
	%ProfileSelector.add_item("None")
	for p in Backend.get_lootfilter_module().get_available_profiles():
		%ProfileSelector.add_item(p)

func _on_filter_profile_dialog_confirmed() -> void:
	if %FilterProfileName.text.is_empty():
		return
	if m_filter_profile_duplicate:
		Backend.get_lootfilter_module().duplicate_selected_profile(%FilterProfileName.text)
	else:
		Backend.get_lootfilter_module().create_profile(%FilterProfileName.text)
	%FilterProfileDialog.hide()

func _on_filter_profile_dialog_visibility_changed() -> void:
	if %FilterProfileDialog.visible:
		%FilterProfileName.clear()

func _on_profile_selector_item_selected(index: int) -> void:
	%DeleteProfileBtn.disabled = index == 0
	%DuplicateProfileBtn.disabled = index == 0
	#%FilterList.disabled = index == 0
	%AddFilterBtn.disabled = index == 0
	
	if index == 0:
		Backend.get_lootfilter_module().clear()
	else:
		Backend.get_lootfilter_module().load_profile(%ProfileSelector.get_item_text(index))


func _on_create_profile_btn_pressed() -> void:
	m_filter_profile_duplicate = false
	%FilterProfileDialog.visible = true

func _on_delete_profile_btn_pressed() -> void:
	if %ProfileSelector.selected <= 0:
		return
	Backend.get_lootfilter_module().delete_profile(
		%ProfileSelector.get_item_text(%ProfileSelector.selected))


func _on_duplicate_profile_btn_pressed() -> void:
	m_filter_profile_duplicate = true
	%FilterProfileDialog.visible = true
