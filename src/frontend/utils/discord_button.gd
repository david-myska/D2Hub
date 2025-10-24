extends Button

const c_discord_name := "TODO"

func _on_pressed() -> void:
	DisplayServer.clipboard_set(c_discord_name)
	text = "Copied!"
	$Timer.start()


func _on_timer_timeout() -> void:
	text = c_discord_name
