extends FoldableContainer

var m_module : Module = null
var m_header_label : Label = null

func _ready() -> void:
	assert(m_module)
	var lbl := Label.new()
	lbl.text = "ON"
	lbl.modulate = Color.GREEN
	add_title_bar_control(lbl)
	m_header_label = lbl
	
	if not m_module.can_be_disabled_manually():
		%DisableBtn.disabled = true
		%DisableBtn.tooltip_text = "This module cannot be disabled manually"
	
	m_module.status_changed.connect(func(status : Module.Status, reason : String):
		if status == Module.Status.RUNNING:
			m_header_label.text = "ON"
			m_header_label.modulate = Color.GREEN
		else:
			m_header_label.text = "OFF"
			m_header_label.modulate = Color.RED
		%Reason.text = reason
	)

func _on_disable_btn_toggled(toggled_on: bool) -> void:
	m_module.disable_manually(toggled_on)


func _on_restart_btn_pressed() -> void:
	pass # Replace with function body.
