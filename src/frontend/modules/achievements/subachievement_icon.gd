extends Control

signal clicked()

var m_in_focus := false

var m_steady_modulate := 0.5
var m_active_modulate := 0.8

func _ready() -> void:
	await get_tree().process_frame
	%Icon.pivot_offset = size / 2

func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		clicked.emit()

func from_achievement(achievement : Achievement):
	achievement.status_changed.connect(_recolor)
	_recolor(achievement.get_status())

func _recolor(status : Achievement.Status):
	if status == Achievement.Status.COMPLETED:
		%Icon.texture = preload("res://assets/blood_pentagram.png")
		m_steady_modulate = 1.0
		m_active_modulate = 1.0
		%Icon.modulate.a = 1.0


func _on_mouse_entered() -> void:
	%Icon.scale = Vector2.ONE * 1.2
	%Icon.modulate.a = m_active_modulate


func _on_mouse_exited() -> void:
	if m_in_focus:
		return
	%Icon.scale = Vector2.ONE
	%Icon.modulate.a = m_steady_modulate


func _on_focus_entered() -> void:
	m_in_focus = true


func _on_focus_exited() -> void:
	m_in_focus = false
	%Icon.scale = Vector2.ONE
	%Icon.modulate.a = m_steady_modulate
