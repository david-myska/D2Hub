extends VBoxContainer

var m_tracked_achis := {}

func track_achievement(achi : Achievement):
	var entry := preload("res://modules/achievements/achievement_overlay_entry.tscn").instantiate()
	entry.from_achievement(achi)
	m_tracked_achis[achi] = entry
	add_child(entry)

func stop_tracking_achievement(achi : Achievement):
	if m_tracked_achis.has(achi):
		m_tracked_achis[achi].queue_free()
		m_tracked_achis.erase(achi)
