extends MarginContainer


func _ready() -> void:
	Backend.get_lootfilter_module().filters_changed.connect(_update_filters)

func _update_filters():
	for c in %FilterList.get_children():
		c.queue_free()
	
	var i := 0
	for mf in Backend.get_lootfilter_module().get_filters():
		var f = preload("res://screens/lootfilter/filter.tscn").instantiate()
		f.m_metadata = mf.get_metadata()
		f.m_metadata.notification_path = f.m_metadata.name# TMP for testing
		f.delete_requested.connect(Backend.get_lootfilter_module().remove_filter.bind(i))
		%FilterList.add_child(f)
		i += 1

func _on_add_filter_pressed() -> void:
	%SetupFilterDialog.show()
