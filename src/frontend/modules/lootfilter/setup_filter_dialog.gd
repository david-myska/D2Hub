extends AcceptDialog

func _ready() -> void:
	visibility_changed.connect(self.reset)
	$VBoxContainer/ScrollContainer/VBoxContainer/AttributeFilter.populate(["shit1", "shit2"])

func reset():
	pass
