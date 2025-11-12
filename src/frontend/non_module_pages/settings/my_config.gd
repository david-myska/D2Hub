class_name MyConfig
extends ConfigFile

signal changed(section : String, key : String, to : Variant)

func Set(section: String, key: String, value: Variant) -> void:
	set_value(section, key, value)
	changed.emit(section, key, value)

func Get(section: String, key: String, default: Variant = null) -> Variant:
	return get_value(section, key, default)
