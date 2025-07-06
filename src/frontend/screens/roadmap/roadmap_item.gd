extends Control

@export var m_expected_release : String = ""
@export var m_title : String = ""
@export var m_description : Array[String] = [""]

func _ready() -> void:
	%ExpectedRelease.text = m_expected_release
	%Title.text = m_title
	var first := true
	for d in m_description:
		if first:
			%Description.text = d
			first = false
		else:
			%Description.text += "\n- " + d
