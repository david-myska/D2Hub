extends MarginContainer

func _ready() -> void:
	Backend.get_bestiary_module().npcs_changed.connect(_update_npcs)

func _update_npcs():
	%MonsterList.fill(Backend.get_bestiary_module().get_monsters())
	%CompanionList.fill(Backend.get_bestiary_module().get_companions())
