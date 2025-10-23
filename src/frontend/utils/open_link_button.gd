extends Button

@export var m_link : String = ""

func _ready() -> void:
	assert(not m_link.is_empty())
	if not m_link.begins_with("https://"):
		m_link = "https://" + m_link

func _on_pressed() -> void:
	OS.shell_open(m_link)
