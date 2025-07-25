extends AcceptDialog

signal validated(stat_id : int, stat_name : String)

var m_stat_id : int = 0

func _ready() -> void:
	confirmed.connect(validate)

func show_filled(stat_id : int, stat_name : String):
	_reset()
	m_stat_id = stat_id
	%NameLE.text = stat_name
	show()
	%NameLE.edit()
	%NameLE.select_all()

func validate():
	%NameLabel.modulate = Color.RED if %NameLE.text.is_empty() else Color.WHITE
	if %NameLE.text.is_empty():
		return
	validated.emit(m_stat_id, %NameLE.text)
	hide()

func _reset():
	%NameLabel.modulate = Color.WHITE
	%NameLE.text = ""
