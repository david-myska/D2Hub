class_name MyConfig
extends ConfigFile

signal changed(section : String, key : String, to : Variant)

func Set(section: String, key: String, value: Variant, force_save : bool = true) -> void:
	set_value(section, key, value)
	changed.emit(section, key, value)
	if force_save:
		save(Cfg.file_path)

func Get(section: String, key: String, default: Variant = null) -> Variant:
	return get_value(section, key, default)
