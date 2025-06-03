extends MarginContainer


func _on_about_label_meta_clicked(meta: Variant) -> void:
	OS.shell_open(str(meta))
