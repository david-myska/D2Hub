extends HBoxContainer

signal item_fix_requested()
signal load_stats_requested()

func from_item(item : Dictionary):
	%ItemName.text = item["name"]
	%Location.text = item["location"]
	%Position.text = "[%s]" % str(item["position"])


func _on_fix_item_btn_pressed() -> void:
	item_fix_requested.emit()


func _on_load_stats_btn_pressed() -> void:
	load_stats_requested.emit()
