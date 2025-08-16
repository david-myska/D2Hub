extends GridContainer

func _ready() -> void:
	Backend.get_statistics_module().dmg_changed.connect(_update_dmg_meter)

func _update_dmg_meter():
	var d := Backend.get_statistics_module().get_dmg_stats()
	%DmgTotal.text = "%s" % d["dmg_total"]
	%DpsTotal.text = "%s" % d["dps"]
	
