
include(FetchContent)

FetchContent_Declare(
  godot-cpp
  GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
  GIT_TAG        4.4
)
FetchContent_MakeAvailable(godot-cpp)

set(SOURCE_FILES
				"gdexample.cpp"
				"register_types.cpp"
)

add_library(libgdexample SHARED ${SOURCE_FILES})

target_include_directories(libgdexample PUBLIC ${godot-cpp_SOURCE_DIR}/include)
target_link_libraries(libgdexample PUBLIC godot-cpp)

install(TARGETS libgdexample DESTINATION ${D2E_INSTALL_LIB_DIR})