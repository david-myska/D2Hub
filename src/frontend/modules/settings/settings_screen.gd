extends MarginContainer

# TODO TMP
signal fix_overlay(r : Rect2i)
signal overlay_edit_mode(enabled : bool)

@onready var m_current_le: LineEdit = null

func _ready() -> void:
	$DirDialog.dir_selected.connect(func(dir_path):
		m_current_le.text = dir_path
		m_current_le.text_changed.emit(dir_path)
	)
	%AutoAttach.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_auto_attach)
	%SkipWarning.button_pressed = App.Config.Get(Cfg.sec_global, Cfg.key_skip_warning)
	var update_rate : int = App.Config.Get(Cfg.sec_global, Cfg.key_updates_per_second)
	%UpdateRateSB.value = update_rate
	Backend.set_update_rate(update_rate)
	
	%SaveDirLineEdit.text_changed.connect(func(new_text):
		Backend.get_backup_module().initialize(new_text)
		App.Config.Set(Cfg.sec_backup, Cfg.key_saves_dir, new_text)
	)
	%SaveDirLineEdit.text = App.Config.Get(Cfg.sec_backup, Cfg.key_saves_dir)
	Backend.get_backup_module().initialize(%SaveDirLineEdit.text)
	%AutoBackup.button_pressed = App.Config.Get(Cfg.sec_backup, Cfg.key_auto_backup)
	
	%EnableOverlay.button_pressed = App.Config.Get(Cfg.sec_overlay, Cfg.key_overlay_enabled, false)

func _on_about_label_meta_clicked(meta: Variant) -> void:
	OS.shell_open(str(meta))


func _on_auto_attach_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_auto_attach, toggled_on)

func _on_skip_warning_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_global, Cfg.key_skip_warning, toggled_on)


func _on_open_save_dir_dialog_btn_pressed() -> void:
	m_current_le = %SaveDirLineEdit
	$DirDialog.popup_centered()


func _on_auto_backup_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_backup, Cfg.key_auto_backup, toggled_on)


func _on_remove_all_backups_pressed() -> void:
	Backend.get_backup_module().delete_all_backups()


func _on_fix_overlay_position_btn_pressed() -> void:
	var r := Backend.get_target_rect()
	fix_overlay.emit(r)


func _on_overlay_edit_mode_btn_toggled(toggled_on: bool) -> void:
	overlay_edit_mode.emit(toggled_on)


func _on_enable_overlay_toggled(toggled_on: bool) -> void:
	App.Config.Set(Cfg.sec_overlay, Cfg.key_overlay_enabled, toggled_on)


func _on_update_rate_sb_value_changed(value: float) -> void:
	@warning_ignore("narrowing_conversion")
	var update_rate : int = value
	App.Config.Set(Cfg.sec_global, Cfg.key_updates_per_second, update_rate)
	Backend.set_update_rate(update_rate)
