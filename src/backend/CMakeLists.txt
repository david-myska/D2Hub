
include(FetchContent)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH x86_32)
else()
    set(ARCH x86_64)
endif()

FetchContent_Declare(
  godot-cpp
  GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
  GIT_TAG        4.4
)
FetchContent_MakeAvailable(godot-cpp)

FetchContent_Declare(
  game_enhancer
  GIT_REPOSITORY https://github.com/david-myska/GameEnhancer.git
  GIT_TAG        eventBasedAchievements
)
FetchContent_MakeAvailable(game_enhancer)


set(SOURCE_FILES
				"binding/gdexample.cpp"
				"binding/register_types.cpp"
				"d2/achievements/achievements.cpp"
				"d2/utilities/utilities.cpp"
)

add_library(libgdexample SHARED ${SOURCE_FILES})

target_include_directories(libgdexample PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${godot-cpp_SOURCE_DIR}/include PUBLIC ${game_enhancer_SOURCE_DIR}/include)
target_link_libraries(libgdexample PUBLIC godot-cpp PUBLIC game_enhancer)

install(TARGETS libgdexample DESTINATION ${D2E_INSTALL_LIB_DIR})