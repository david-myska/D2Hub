extends MarginContainer

@onready var m_auto_attach: CheckButton = %AutoAttach

func _ready() -> void:
	m_auto_attach.button_pressed = App.Config.get_value(Cfg.sec_global, Cfg.key_auto_attach)

func _on_about_label_meta_clicked(meta: Variant) -> void:
	OS.shell_open(str(meta))


func _on_auto_attach_toggled(toggled_on: bool) -> void:
	App.Config.set_value(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)
