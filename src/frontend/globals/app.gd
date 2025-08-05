extends Node

@onready var Config := _make_default_config()

func _ready() -> void:
	if Config.load(Cfg.file_path) != OK:
		push_warning("Loading of '%s' failed" % Cfg.file_path)

func _exit_tree() -> void:
	Config.save(Cfg.file_path)

func _make_default_config() -> MyConfig:
	var cfg := MyConfig.new()
	cfg.Set(Cfg.sec_global, Cfg.key_skip_warning, false)
	cfg.Set(Cfg.sec_global, Cfg.key_auto_attach, false)
	cfg.Set(Cfg.sec_global, Cfg.key_updates_per_second, int(5))
	cfg.Set(Cfg.sec_backup, Cfg.key_saves_dir, OS.get_environment("APPDATA") + r"\MedianXL\save")
	cfg.Set(Cfg.sec_backup, Cfg.key_auto_backup, false)
	cfg.Set(Cfg.sec_overlay, Cfg.key_overlay_enabled, false)
	return cfg
