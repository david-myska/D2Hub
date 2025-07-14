extends VBoxContainer

func set_message(message : String, color : Color = Color.WHITE, duration : float = 5.0):
	%MessageLbl.text = message
	modulate = color
	$Timer.wait_time = duration

func _on_timer_timeout() -> void:
	queue_free()
