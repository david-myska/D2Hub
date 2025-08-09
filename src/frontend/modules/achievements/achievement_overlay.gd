extends VBoxContainer

func _ready() -> void:
	%ConditionsView.m_columns = 1

func track_achievement(achi : Achievement):
	visible = true
	%AchiName.text = achi.get_metadata()["name"]
	%ConditionsView.from_achievement(achi)

func reset():
	visible = false
