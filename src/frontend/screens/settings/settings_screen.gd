extends MarginContainer

@onready var m_current_le: LineEdit = null

func _ready() -> void:
	$DirDialog.dir_selected.connect(func(dir_path):
		m_current_le.text = dir_path
		m_current_le.text_changed.emit(dir_path)
	)
	%MXLDirLineEdit.text_changed.connect(func(new_text):
		if not Backend.is_mxl_dir_valid(new_text):
			return
		Backend.initialize_backend(new_text)
		App.Config.Set(Cfg.sec_global, Cfg.key_mxl_dir, new_text)
	)
	%MXLDirLineEdit.text = App.Config.Get(Cfg.sec_global, Cfg.key_mxl_dir)
	%AutoAttach.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach)
	%SkipWarning.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_skip_warning)
	
	%SaveDirLineEdit.text_changed.connect(func(new_text):
		Backend.initialize_saves_backup(new_text)
		App.Config.Set(Cfg.sec_backup, Cfg.key_saves_dir, new_text)
	)
	%SaveDirLineEdit.text = App.Config.Get(Cfg.sec_backup, Cfg.key_saves_dir)
	%AutoBackup.button_pressed = App.Config.Get(Cfg.sec_backup, Cfg.key_auto_backup)

func _on_about_label_meta_clicked(meta: Variant) -> void:
	OS.shell_open(str(meta))


func _on_auto_attach_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)


func _on_open_mxl_dir_dialog_btn_pressed() -> void:
	m_current_le = %MXLDirLineEdit
	$DirDialog.popup_centered()


func _on_skip_warning_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)


func _on_open_save_dir_dialog_btn_pressed() -> void:
	m_current_le = %SaveDirLineEdit
	$DirDialog.popup_centered()
