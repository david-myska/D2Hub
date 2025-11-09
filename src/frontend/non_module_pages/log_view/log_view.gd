extends MarginContainer

func _ready() -> void:
	Backend.add_log_entry.connect(_add_log_entry)

func _add_log_entry(msg : String, type : int):
	var type_text := "INFO" if type == Backend.INFO else "WARN" if type == Backend.WARNING else "ERROR"
	$RichTextLabel.append_text("[%s]\t %s\n" % [type_text, msg])
