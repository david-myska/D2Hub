extends HBoxContainer

func from_monster(d : Dictionary):
	%FireRes.text = str(d["fire_res"])
	%ColdRes.text = str(d["cold_res"])
	%LightningRes.text = str(d["light_res"])
	%PoisonRes.text = str(d["pois_res"])
	%FireAbs.text = str(d["fire_abs"])
	%ColdAbs.text = str(d["cold_abs"])
	%LightningAbs.text = str(d["light_abs"])
	%PoisonAbs.text = str(d["pois_abs"])
