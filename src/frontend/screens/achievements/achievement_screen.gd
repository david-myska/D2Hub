extends MarginContainer

@onready var m_achis: VBoxContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiView/VBoxContainer
@onready var m_details: MarginContainer = $VBoxContainer/MarginContainer/HBoxContainer/AchiDetail/AchievementDetail
@onready var m_categories: VBoxContainer = $VBoxContainer/MarginContainer/HBoxContainer/ScrollContainer/Categories

const c_category_types := [
	"All", "Act 1", "Act 2", "Act 3", "Act 4", "Act 5",
	"Global", "Dungeons", "Rifts", "Others"
]
var m_cat2views : Dictionary = {}
var m_cat_btn_grp := ButtonGroup.new()

func _ready() -> void:
	fill_achievements()
	Backend.new_achievements_loaded.connect(fill_achievements)

func status_to_str(s : Achievement.Status) -> String:
	match s:
		Achievement.Status.DISABLED: return "Disabled"
		Achievement.Status.INACTIVE: return "Inactive"
		Achievement.Status.ACTIVE: return "Active"
		Achievement.Status.COMPLETED: return "Completed"
		Achievement.Status.FAILED: return "Failed"
	return "Unknown"

func _report_status(status : Achievement.Status, achi : Achievement):
	print("%s - Changed status to -> %s" % [achi.get_metadata()["name"], status_to_str(status)])

func filter_achis(category : String) -> void:
	for a in m_achis.get_children():
		a.visible = false
	for a in m_cat2views[category]:
		a.visible = true

func add_category(cat : String) -> void:
	var btn := Button.new()
	btn.text = cat
	btn.toggle_mode = true
	btn.button_group = m_cat_btn_grp
	m_categories.add_child(btn)
	btn.toggled.connect(func(is_down : bool):
		if is_down:
			filter_achis(cat)
	)

func create_categories(achis : Array) -> void:
	for c in c_category_types:
		if m_cat2views[c].is_empty():
			continue
		add_category(c)
	m_categories.get_child(0).button_pressed = true

func reset() -> void:
	for c in m_achis.get_children():
		c.queue_free()
	m_cat2views.clear()
	for cat in c_category_types:
		m_cat2views[cat] = []
	for c in m_categories.get_children():
		c.queue_free()

func fill_achievements() -> void:
	var achis := Backend.get_achievements()
	reset()
	for a in achis:
		var achi_view = preload("res://screens/achievements/achievement_view.tscn").instantiate()
		achi_view.from_achievement(a)
		m_achis.add_child(achi_view)
		achi_view.clicked.connect(m_details.from_achievement.bind(a))
		a.status_changed.connect(_report_status.bind(a))
		var category = a.get_metadata()["category"]
		if category in m_cat2views and category != "All":
			m_cat2views[category].append(achi_view)
		else:
			m_cat2views["Others"].append(achi_view)
		m_cat2views["All"].append(achi_view)
	create_categories(achis)
