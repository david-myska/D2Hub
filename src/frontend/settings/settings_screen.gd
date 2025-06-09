extends MarginContainer

signal auto_attach_changed(on : bool)

@onready var m_mxl_dir: LineEdit = %MXLDirLineEdit

func _ready() -> void:
	$DirDialog.dir_selected.connect(func(dir_path):
		m_mxl_dir.text = dir_path
		m_mxl_dir.text_changed.emit(dir_path)
	)
	m_mxl_dir.text_changed.connect(func(new_text):
		if not Backend.is_mxl_dir_valid(new_text):
			return
		Backend.initialize_backend(new_text)
		App.Config.set_value(Cfg.sec_global, Cfg.key_mxl_dir, new_text)
	)
	m_mxl_dir.text = App.Config.get_value(Cfg.sec_global, Cfg.key_mxl_dir)
	%AutoAttach.button_pressed = App.Config.get_value(Cfg.sec_global, Cfg.key_auto_attach)
	%SkipWarning.button_pressed = App.Config.get_value(Cfg.sec_global, Cfg.key_skip_warning)

func _on_about_label_meta_clicked(meta: Variant) -> void:
	OS.shell_open(str(meta))


func _on_auto_attach_toggled(toggled_on: bool) -> void:
	App.Config.set_value(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)
	auto_attach_changed.emit(toggled_on)


func _on_open_dir_dialog_btn_pressed() -> void:
	$DirDialog.popup_centered()


func _on_skip_warning_toggled(toggled_on: bool) -> void:
	App.Config.set_value(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)
