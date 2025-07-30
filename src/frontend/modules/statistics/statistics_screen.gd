extends MarginContainer

func _ready() -> void:
	Backend.get_statistics_module().statistics_changed.connect(_update_statistics)

func _update_statistics():
	var d := Backend.get_statistics_module().get_statistics()
	%ExpPer.text = "%s per 2min" % d["exp_per"]
	%ExpTotal.text = "(%s)" % d["exp_total"]
	%ItemsPer.text = "%s per 2min" % d["items_per"]
	%ItemsTotal.text = "(%s)" % d["items_total"]
	%RaresPer.text = "%s per 2min" % d["rare_per"]
	%RaresTotal.text = "(%s)" % d["rare_total"]
	%SetsPer.text = "%s per 2min" % d["set_per"]
	%SetsTotal.text = "(%s)" % d["set_total"]
	%UniquesPer.text = "%s per 2min" % d["unique_per"]
	%UniquesTotal.text = "(%s)" % d["unique_total"]


func _on_button_pressed() -> void:
	Backend.get_statistics_module().reset()
