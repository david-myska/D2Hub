extends AcceptDialog

func _ready() -> void:
	visibility_changed.connect(self.reset)

func reset():
	pass
