extends Window

var m_hwnd : int = 0

func _ready() -> void:
	_initialize()
	visibility_changed.connect(_initialize)

func enable(enable : bool = true):
	if not enable:
		enable_edit_mode(false)
	visible = enable
	_initialize()

func _initialize():
	if not visible:
		return
	m_hwnd = DisplayServer.window_get_native_handle(
		DisplayServer.WINDOW_HANDLE, get_window_id())
	Backend.enable_window_clickthrough(m_hwnd, true)

func enable_edit_mode(enable : bool = true):
	if not visible:
		return
	if $EditMode.visible == enable:
		return
	$EditMode.visible = enable
	mouse_passthrough = not enable
	for p in $Panels.get_children():
		p.enable_edit_mode(enable)
	Backend.enable_window_clickthrough(m_hwnd, not enable)

func fill(r : Rect2i):
	position = r.position
	size = r.size

func add_panel(panel_name : String, content : Control):
	var p := preload("res://overlay/overlay_panel.tscn").instantiate()
	p.m_panel_name = panel_name
	p.request_change.connect(_modify_panel.bind(p))
	p.set_content(content)
	$Panels.add_child(p)

func _modify_panel(r : Rect2, panel : Control):
	if not Rect2(Vector2.ZERO, size).encloses(r):
		return
	panel.position = r.position
	panel.size = r.size
