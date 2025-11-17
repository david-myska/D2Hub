extends Button

@onready var http := $HTTPRequest

func _ready():
	await get_tree().create_timer(2.0).timeout
	check_latest_release()

func check_latest_release():
	Backend.log("UpdateChecker - Checking for updates")
	var url = "https://api.github.com/repos/david-myska/D2Hub/releases/latest"
	var headers = ["User-Agent: D2Hub_update_checker"]
	http.request(url, headers)


func _on_http_request_completed(_result, response_code: int, _headers, body: PackedByteArray) -> void:
	if response_code != 200:
		Backend.log("UpdateChecker - query failed: %s" % response_code)
		return
	
	var json = JSON.parse_string(body.get_string_from_utf8())
	if json == null:
		Backend.log("UpdateChecker - JSON parse error")
		return
	
	var latest_tag : String = json["tag_name"]
	latest_tag = latest_tag.trim_prefix("v")
	Backend.log("UpdateChecker - Latest version: %s" % latest_tag)
	
	var current_version : String = ProjectSettings.get_setting("application/config/version")
	# TODO TMP for beta
	current_version = "1.0-beta.1"
	
	if latest_tag != current_version:
		text = "New version available: %s" % latest_tag
		disabled = false
		modulate = Color.ORANGE_RED
	else:
		text = "Up to date"


func _on_pressed() -> void:
	OS.shell_open("https://github.com/david-myska/D2Hub/releases/latest")
