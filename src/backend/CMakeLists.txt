
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
				"binding/d2hub_backend.cpp"
				"binding/register_types.cpp"
				"d2/achievements/achievements.cpp"
				"d2/utilities/utilities.cpp"
)

add_library(d2hub_backend SHARED ${SOURCE_FILES})

target_include_directories(d2hub_backend PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${godot-cpp_SOURCE_DIR}/include PUBLIC ${game_enhancer_SOURCE_DIR}/include)
target_link_libraries(d2hub_backend PUBLIC godot-cpp PUBLIC game_enhancer)

install(TARGETS d2hub_backend DESTINATION ${D2E_INSTALL_LIB_DIR})
install(FILES $<TARGET_FILE:d2hub_backend> DESTINATION ${D2E_FRONTEND_REQ_DIR})