extends AcceptDialog

signal validated(stat_id : int, stat_name : String, categories : String)

var m_stat_id : int = 0

func _ready() -> void:
	confirmed.connect(validate)

func show_filled(stat_id : int, stat_name : String, categories : String):
	_reset()
	m_stat_id = stat_id
	%NameLE.text = stat_name
	%CategoriesLE.text = categories
	show()

func validate():
	%NameLabel.modulate = Color.RED if %NameLE.text.is_empty() else Color.WHITE
	%CategoriesLabel.modulate = Color.RED if %CategoriesLE.text.is_empty() else Color.WHITE
	if %NameLE.text.is_empty() or %CategoriesLE.text.is_empty():
		return
	validated.emit(m_stat_id, %NameLE.text, %CategoriesLE.text)
	hide()

func _reset():
	%NameLabel.modulate = Color.WHITE
	%NameLE.text = ""
	%CategoriesLabel.modulate = Color.WHITE
	%CategoriesLE.text = ""
