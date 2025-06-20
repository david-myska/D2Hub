extends VBoxContainer

var m_module : Module

func from_module(module : Module) -> void:
	m_module = module
	%ManualDisable.disabled = not m_module.can_be_disabled_manually()
	m_module.status_changed.connect(func(status : Module.Status, reason : String):
		%Running.button_pressed = status == Module.ENABLED
		%Reason.text = reason
	)

func _on_manual_disable_toggled(toggled_on: bool) -> void:
	m_module.disable_manually(toggled_on)
