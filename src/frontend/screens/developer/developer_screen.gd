extends MarginContainer

var m_dev : DeveloperControl = Backend.get_developer_control()

var m_locked_pos : Vector2i = Vector2i.ZERO
var m_rel_pos : Vector2i = Vector2i.ZERO
var m_rel_pos2 : Vector2i = Vector2i.ZERO

func _on_lock_pos_btn_pressed() -> void:
	m_locked_pos = m_dev.get_player_position()
	$HBoxContainer/GridContainer/LockedPos.text = str(m_locked_pos)
	_update_rel(m_rel_pos)

func _update_rel(to_point : Vector2i):
	m_rel_pos = to_point
	$HBoxContainer/GridContainer/RelativePos.text = str(m_rel_pos - m_locked_pos)

func _on_relative_pos_btn_pressed() -> void:
	_update_rel(m_dev.get_player_position())
	_update_rel2(m_rel_pos2)

func _update_rel2(to_point : Vector2i):
	m_rel_pos2 = to_point
	$HBoxContainer/GridContainer/RelativePos2.text = str(m_rel_pos2 - m_rel_pos)

func _on_relative_pos_btn_2_pressed() -> void:
	_update_rel2(m_dev.get_player_position())
