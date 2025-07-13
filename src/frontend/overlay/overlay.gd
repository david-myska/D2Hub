extends Window

var m_hwnd : int = 0

func _ready() -> void:
	m_hwnd = DisplayServer.window_get_native_handle(
		DisplayServer.WINDOW_HANDLE, get_window_id())
	Backend.enable_window_clickthrough(m_hwnd, true)

func enable_edit_mode(enable : bool = true):
	$EditMode.visible = enable
	mouse_passthrough = not enable
	for p in $Panels.get_children():
		p.enable_edit_mode(enable)
	Backend.enable_window_clickthrough(m_hwnd, not enable)

func fill(r : Rect2i):
	position = r.position
	size = r.size

func add_panel(panel_name : String):
	var p := preload("res://overlay/overlay_panel.tscn").instantiate()
	p.m_panel_name = panel_name
	p.request_change.connect(_modify_panel.bind(p))
	$Panels.add_child(p)

func _modify_panel(r : Rect2, panel : Control):
	if not Rect2(Vector2.ZERO, size).encloses(r):
		return
	panel.position = r.position
	panel.size = r.size
