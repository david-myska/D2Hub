extends MarginContainer

func _ready() -> void:
	Backend.get_bestiary_module().monsters_changed.connect(_update_monsters)

func _update_monsters():
	%MonsterList.fill(Backend.get_bestiary_module().get_monsters())
