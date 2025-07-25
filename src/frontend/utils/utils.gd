class_name Utils

static func ItemColor(item : Dictionary) -> Color:
	if item.has("name") and item["name"] == "{[( Unknown )]}":
		return Color.ORANGE_RED
	match item.get("quality", 0):
		MetaFilter.Quality.LOW: return Color.WHITE
		MetaFilter.Quality.NORMAL: return Color.WHITE
		MetaFilter.Quality.SUPERIOR: return Color.WHITE
		MetaFilter.Quality.MAGIC: return Color.BLUE
		MetaFilter.Quality.SET: return Color.GREEN
		MetaFilter.Quality.RARE: return Color.YELLOW
		MetaFilter.Quality.UNIQUE: return Color.GOLDENROD
		_: return Color.RED
