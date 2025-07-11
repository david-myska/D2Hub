extends HBoxContainer

signal delete_requested()

var m_metadata : FilterMetadata :
	set(m):
		m_metadata = m
		%NameLbl.text = m_metadata.name
		%ActiveCheckBtn.button_pressed = m_metadata.active

func _on_delete_btn_pressed() -> void:
	delete_requested.emit()
