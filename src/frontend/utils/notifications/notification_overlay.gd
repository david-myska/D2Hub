extends VBoxContainer

func _ready() -> void:
	Backend.get_notifier().pushed.connect(push)

func to_color(type : int) -> Color:
	match type:
		Notifier.INFO:
			return Color.WHITE
		Notifier.WARNING:
			return Color.YELLOW
		Notifier.ERROR:
			return Color.RED
		_:
			return Color.BLUE

func push(type : int, message : String, targets : int = Notifier.Target.ALL, duration : float = 5.0):
	if not (targets & Notifier.Target.OVERLAY):
		return
	var msg := preload("res://utils/notifications/overlay_message.tscn").instantiate()
	msg.set_message(message, to_color(type), duration)
	add_child(msg)
	move_child(msg, 0)
