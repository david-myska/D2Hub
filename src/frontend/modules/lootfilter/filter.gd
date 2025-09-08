extends HBoxContainer

signal delete_requested()
signal modify_requested()

var m_metadata : FilterMetadata :
	set(m):
		m_metadata = m
		%NameLbl.text = m_metadata.name
		%ActiveCheckBtn.button_pressed = m_metadata.active

func _on_delete_btn_pressed() -> void:
	delete_requested.emit()


func _on_modify_btn_pressed() -> void:
	modify_requested.emit()


func _on_active_check_btn_toggled(toggled_on: bool) -> void:
	m_metadata.active = toggled_on
