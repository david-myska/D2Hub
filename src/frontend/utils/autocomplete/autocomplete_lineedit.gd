extends LineEdit

signal choice_selected(choice : String)

@export var autocomplete_choices : Array = [] :
	set(a):
		autocomplete_choices = a
		_fill_choices()
@export var fill_text_on_choice : bool = true

func _ready() -> void:
	autocomplete_choices = ["22", "11", "121", "321", "12", "123", "1232132213"]

func _fill_choices() -> void:
	for c in %Choices.get_children():
		c.queue_free()
	for s in autocomplete_choices:
		var btn := Button.new()
		btn.flat = true
		btn.alignment = HORIZONTAL_ALIGNMENT_LEFT
		btn.text = s
		btn.pressed.connect(_on_choice_accepted.bind(s))
		%Choices.add_child(btn)


func _on_choice_accepted(choice : String) -> void:
	text = choice
	$AutoCompleteMenu.visible = false
	choice_selected.emit(choice)


func _on_update_choices_timer_timeout() -> void:
	for c in %Choices.get_children():
		c.visible = text in c.text


func _on_text_changed(_new_text: String) -> void:
	$UpdateChoicesTimer.start()


func _on_focus_entered() -> void:
	$AutoCompleteMenu.visible = true


func _on_focus_exited() -> void:
	$AutoCompleteMenu.visible = false
