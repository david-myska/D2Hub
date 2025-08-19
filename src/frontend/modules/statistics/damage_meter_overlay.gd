extends GridContainer

func _ready() -> void:
	Backend.get_statistics_module().dmg_changed.connect(_update_dmg_meter)

func _update_dmg_meter():
	var d := Backend.get_statistics_module().get_dmg_stats()
	%BurstDmgTotal.text = _format_number(d["dmg_burst"])
	%BurstDmgRecord.text = "(%s)" % _format_number(d["dmg_burst_record"])
	%StableDpsTotal.text = _format_number(d["dps_stable"])
	%StableDpsRecord.text = "(%s)" % _format_number(d["dps_stable_record"])
	%DmgTotal.text = _format_number(d["dmg_total"])

func _format_number(num: float) -> String:
	var abs_num = absf(num)
	if abs_num >= 1_000_000_000.0:
		return "%.2fB" % (num / 1_000_000_000.0)
	elif abs_num >= 1_000_000.0:
		return "%.2fM" % (num / 1_000_000.0)
	elif abs_num >= 1_000.0:
		return "%.2fK" % (num / 1_000.0)
	else:
		return str(int(num))
