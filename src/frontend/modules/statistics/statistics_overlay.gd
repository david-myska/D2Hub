extends VBoxContainer

func _ready() -> void:
	Backend.get_statistics_module().statistics_changed.connect(_update_statistics)

func _update_statistics():
	var d := Backend.get_statistics_module().get_statistics()
	%ExpTotal.text = "%s" % d["exp_total"]
	%ExpPer.text = "%s /min" % d["exp_per"]
	%ItemsTotal.text = "%s" % d["items_total"]
	%ItemsPer.text = "%s /min" % d["items_per"]
	%RaresTotal.text = "%s" % d["rare_total"]
	%RaresPer.text = "%s /min" % d["rare_per"]
	%SetsTotal.text = "%s" % d["set_total"]
	%SetsPer.text = "%s /min" % d["set_per"]
	%UniquesTotal.text = "%s" % d["unique_total"]
	%UniquesPer.text = "%s /min" % d["unique_per"]
