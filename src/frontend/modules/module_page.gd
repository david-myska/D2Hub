extends MarginContainer

@export var m_show_status : bool = true

func fill_overlay_panels(_adder : Callable):
	push_warning("Non-overriden fill_overlay_panels function called in node '%s'" % name)
