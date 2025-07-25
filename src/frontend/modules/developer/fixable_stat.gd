extends HBoxContainer

signal stat_fix_requested()

func from_stat(stat : Dictionary):
	var id : int = stat["id"]
	%StatId.text = "[%x:%x]" % [id & 0xFFFF, (id >> 16) & 0xFFFF]
	%StatName.text = stat["name"]
	%StatValue.text = str(stat["value"])

func _on_fix_btn_pressed() -> void:
	stat_fix_requested.emit()
