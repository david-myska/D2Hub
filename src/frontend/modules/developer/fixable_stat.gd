extends HBoxContainer

signal stat_fix_requested()

func from_stat(stat : Dictionary):
	%StatName.text = stat["name"]
	%StatValue.text = str(stat["value"])
	%Categories.text = stat["categories"]

func _on_fix_btn_pressed() -> void:
	stat_fix_requested.emit()
