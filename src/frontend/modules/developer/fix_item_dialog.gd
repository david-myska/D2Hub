extends AcceptDialog

signal validated(item_class : int, item_name : String, tier : int, fix_other_tiers : bool)

var m_item_class : int = 0

func _ready() -> void:
	confirmed.connect(validate)

func show_filled(item_class : int, item_name : String):
	_reset()
	m_item_class = item_class
	%NameLE.text = _remove_tier(item_name)
	show()
	%NameLE.edit()
	%NameLE.select_all()

func validate():
	%NameLabel.modulate = Color.RED if %NameLE.text.is_empty() else Color.WHITE
	if %NameLE.text.is_empty():
		return
	var tier : int = %ChosenTier.get_selected_id() if %IsTiered.button_pressed else 0
	validated.emit(m_item_class, %NameLE.text, tier, %FixOtherTiers.button_pressed)
	hide()

func _remove_tier(item_name : String) -> String:
	if not item_name.ends_with(")"):
		return item_name
	return item_name.substr(0, item_name.rfind("(") - 1)

func _on_is_tiered_toggled(toggled_on: bool) -> void:
	%ChosenTier.disabled = not toggled_on
	%FixOtherTiers.disabled = not toggled_on

func _reset():
	%NameLabel.modulate = Color.WHITE
	%NameLE.text = ""
	%IsTiered.button_pressed = true
	%ChosenTier.select(0)
	%FixOtherTiers.button_pressed = true
