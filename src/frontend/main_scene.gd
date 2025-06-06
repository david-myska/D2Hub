extends Control

@onready var gd_example: GDExample = $GDExample
@onready var discover_btn: Button = $MarginContainer/VBoxContainer/Header/PanelContainer/HBoxContainer/VBoxContainer/DiscoverBtn
@onready var attach_btn: Button = $MarginContainer/VBoxContainer/Header/PanelContainer/HBoxContainer/VBoxContainer/AttachBtn

func _ready() -> void:
	$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer/Developer.m_dev = gd_example.get_developer_control()
	gd_example.target_process_exists.connect(func(exists : bool):
		print("Process exists: ", exists)
		discover_btn.disabled = exists
	)
	gd_example.target_process_attached.connect(func(attached : bool):
		print("Process attached: ", attached)
		if attached:
			attach_btn.text = "Detach"
		else:
			attach_btn.text = "Attach"
	)
	
	$MarginContainer/VBoxContainer/Body/MainPanel/TabContainer/Achievements.fill_achievements(gd_example.get_achievements())

func _on_discover_btn_pressed() -> void:
	gd_example.initialize_backend(r"C:\games\median-xl")
	gd_example.discover_target_process()


func _on_attach_btn_pressed() -> void:
	gd_example.attach_to_target_process(attach_btn.text == "Attach")


func _on_start_btn_pressed() -> void:
	gd_example.start_memory_processor()


func _on_stop_btn_pressed() -> void:
	gd_example.stop_memory_processor()
