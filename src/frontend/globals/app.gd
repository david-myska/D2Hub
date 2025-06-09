extends Node

@onready var Config := _make_default_config()

func _ready() -> void:
	if Config.load(Cfg.file_path) != OK:
		push_warning("Loading of '%s' failed" % Cfg.file_path)

func _exit_tree() -> void:
	Config.save(Cfg.file_path)

func _make_default_config() -> ConfigFile:
	var cfg := ConfigFile.new()
	cfg.set_value(Cfg.sec_global, Cfg.key_skip_warning, false)
	cfg.set_value(Cfg.sec_global, Cfg.key_auto_attach, false)
	cfg.set_value(Cfg.sec_global, Cfg.key_mxl_dir, r"C:\games\median-xl")
	return cfg
