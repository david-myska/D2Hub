extends Node

@onready var Config := ConfigFile.new()

func _ready() -> void:
	if Config.load(Cfg.file_path) != OK:
		_make_default_config(Config)

func _exit_tree() -> void:
	Config.save(Cfg.file_path)

func _make_default_config(cfg : ConfigFile) -> void:
	cfg.set_value(Cfg.sec_global, Cfg.key_auto_attach, false)
	cfg.set_value(Cfg.sec_global, Cfg.key_mxl_dir, r"C:\games\median-xl")
