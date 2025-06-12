extends MarginContainer

@export var m_columns : int = 3

func add_name(cond_name : String) -> void:
	var hbox := HBoxContainer.new()
	hbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_theme_constant_override("Separation", 10)
	var l := HSeparator.new()
	l.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_child(l)
	var lbl := Label.new()
	lbl.text = cond_name
	#var lbl := RichTextLabel.new()
	#lbl.bbcode_enabled = true
	#lbl.fit_content = true
	#lbl.text = "[center]%s[/center]" % cond_name
	#lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_child(lbl)
	var r := HSeparator.new()
	r.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_child(r)
	$VBoxContainer.add_child(hbox)

func add_texts(cond_data : Array) -> void:
	var grid := GridContainer.new()
	grid.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	grid.columns = m_columns
	for d in cond_data:
		var lbl := Label.new()
		lbl.text = d["text"]
		#var lbl := RichTextLabel.new()
		#lbl.fit_content = true
		#lbl.text = d["text"]
		#lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		grid.add_child(lbl)
		d["label"] = lbl
	$VBoxContainer.add_child(grid)

func clear():
	for c in $VBoxContainer.get_children():
		c.queue_free()

func _update_condition(d : Dictionary) -> void:
	d["label"].text = d["text"]
	var color := Color.WHITE
	if d["completed"]:
		color = Color.GREEN
		if d["type"] == Achievement.Condition.FAILER:
			color = Color.RED
	d["label"].modulate = color

func update_conditions(ids : Array, achi : Achievement):
	var by_ids = achi.get_conditions().get_conditions_by_ids()
	for id in ids:
		_update_condition(by_ids[id]) 
