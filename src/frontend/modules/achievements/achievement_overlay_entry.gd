extends VBoxContainer


func from_achievement(achi : Achievement):
	%AchiName.text = achi.get_metadata()["name"]
	%ConditionsView.from_achievement(achi)
