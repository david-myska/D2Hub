extends PopupPanel

func show_message(msg : String) -> void:
	$MarginContainer/Label.text = msg
	popup_centered()
