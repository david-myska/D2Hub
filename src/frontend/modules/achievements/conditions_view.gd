extends MarginContainer

@export var m_columns : int = 3

var m_disconnect_callback : Callable = func(): pass

func from_achievement(achi : Achievement) -> void:
	clear()
	var by_category := achi.get_conditions().get_conditions_by_categories()
	for c in range(Achievement.ALL_CONDITIONS):
		if by_category[c].is_empty():
			continue
		add_name(to_str(c))
		add_texts(by_category[c].values())
	var update_callback = update_conditions.bind(achi)
	achi.progress_made.connect(update_callback)
	m_disconnect_callback = achi.progress_made.disconnect.bind(update_callback)

func add_name(cond_name : String) -> void:
	var hbox := HBoxContainer.new()
	hbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_theme_constant_override("Separation", 10)
	var l := HSeparator.new()
	l.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hbox.add_child(l)
	var lbl := Label.new()
	lbl.text = cond_name
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
		grid.add_child(lbl)
		d["label"] = lbl
		_update_condition(d)
	$VBoxContainer.add_child(grid)

func clear():
	m_disconnect_callback.call()
	m_disconnect_callback = func(): pass
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

func to_str(c : Achievement.Condition) -> String:
	match c:
		Achievement.Condition.PRECONDITION:
			return "Preconditions"
		Achievement.Condition.ACTIVATOR:
			return "Activation"
		Achievement.Condition.COMPLETER:
			return "Completion"
		Achievement.Condition.FAILER:
			return "How to Fail"
		Achievement.Condition.VALIDATOR:
			return "Validates on Completion"
		Achievement.Condition.RESETER:
			return "Resets & allows Activation again"
	return "Invalid condition"
