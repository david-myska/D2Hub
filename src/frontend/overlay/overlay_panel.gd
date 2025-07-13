extends Control

signal request_change(new_rect : Rect2)

@export var m_panel_name : String = ""

var m_edit_mode := false
var m_dragging := false
var m_resizing := false

var m_min_size := Vector2(100, 100)

func _ready() -> void:
	enable_edit_mode()
	%NameLbl.text = m_panel_name
	custom_minimum_size = m_min_size

func enable_edit_mode(enable : bool = true):
	$EditMode.visible = enable
	$Content.visible = not enable
	m_edit_mode = enable

func process_resizing_input(event : InputEvent, move_dir : Vector2, grow_dir : Vector2):
	if not m_edit_mode:
		return
	
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			m_resizing = true
		else:
			m_resizing = false
	elif event is InputEventMouseMotion:
		if m_resizing:
			var new_pos : Vector2 = position + (event.relative * move_dir)
			var new_size : Vector2 = size + (event.relative * grow_dir)
			if new_size.x < m_min_size.x and new_size.y < m_min_size.y:
				return
			if new_size.x < m_min_size.x:
				new_size.x = max(new_size.x, m_min_size.x)
				new_pos.x = position.x
			if new_size.y < m_min_size.y:
				new_size.y = max(new_size.y, m_min_size.y)
				new_pos.y = position.y
			request_change.emit(Rect2(new_pos, new_size))

func process_moving_input(event : InputEvent):
	if not m_edit_mode:
		return
	
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			m_dragging = true
		else:
			m_dragging = false
	elif event is InputEventMouseMotion:
		if m_dragging:
			var new_pos : Vector2 = position + event.relative
			request_change.emit(Rect2(new_pos, size))


func _on_top_left_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.ONE, -Vector2.ONE)


func _on_top_mid_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.DOWN, Vector2.UP)


func _on_top_right_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.DOWN, Vector2.UP + Vector2.RIGHT)


func _on_left_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.RIGHT, Vector2.LEFT)


func _on_mid_gui_input(event: InputEvent) -> void:
	process_moving_input(event)


func _on_right_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.ZERO, Vector2.RIGHT)


func _on_bottom_left_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.RIGHT, Vector2.DOWN + Vector2.LEFT)


func _on_bottom_mid_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.ZERO, Vector2.DOWN)


func _on_bottom_right_gui_input(event: InputEvent) -> void:
	process_resizing_input(event, Vector2.ZERO, Vector2.ONE)
