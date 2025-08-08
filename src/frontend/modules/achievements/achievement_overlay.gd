extends VBoxContainer

func track_achievement(achi : Achievement):
	visible = true
	%AchiName.text = achi.get_metadata()["name"]
	%ConditionsView.from_achievement(achi)

func reset():
	visible = false


func _on_resized() -> void:
	%ConditionsView.m_columns = max(1, size.x / 100)
