extends Window

var m_hwnd : int = 0

var m_enabled : bool = false
var m_target_in_focus : bool = false

func _ready() -> void:
	_initialize()
	visibility_changed.connect(_initialize)

func enable(enable_ : bool = true):
	m_enabled = enable_
	if m_enabled:
		$UpdateTimer.start()
	else:
		$UpdateTimer.stop()
	# Perform initial/finishing update
	_periodic_update()
	if not m_enabled:
		enable_edit_mode(false)

func _initialize():
	if not visible:
		return
	m_hwnd = DisplayServer.window_get_native_handle(
		DisplayServer.WINDOW_HANDLE, get_window_id())
	Backend.enable_window_clickthrough(m_hwnd, true)

func enable_edit_mode(enable_ : bool = true):
	if $EditMode.visible == enable_:
		return
	$EditMode.visible = enable_
	mouse_passthrough = not enable_
	for p in $Panels.get_children():
		p.enable_edit_mode(enable_)
	Backend.enable_window_clickthrough(m_hwnd, not enable_)

func _periodic_update():
	var r : Rect2i = Backend.get_target_rect()
	position = r.position
	size = r.size
	m_target_in_focus = Backend.is_target_window_focused()
	visible = m_enabled and m_target_in_focus
	if not m_enabled:
		enable_edit_mode(false)

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


func _on_update_timer_timeout() -> void:
	_periodic_update()
