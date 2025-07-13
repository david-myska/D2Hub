class_name MyConfig
extends ConfigFile

signal changed(section : String, key : String)

func Set(section: String, key: String, value: Variant) -> void:
	set_value(section, key, value)
	changed.emit(section, key)

func Get(section: String, key: String, default: Variant = null) -> Variant:
	return get_value(section, key, default)
