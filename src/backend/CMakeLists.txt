
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
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.3
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  game_enhancer
  GIT_REPOSITORY https://github.com/david-myska/GameEnhancer.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(game_enhancer)


set(SOURCE_FILES
				"binding/modules/module.cpp"
				"binding/modules/achievements_module.cpp"
				"binding/modules/backup_module.cpp"
				"binding/modules/developer_module.cpp"
				"binding/notifier.cpp"
				"binding/d2hub_backend.cpp"
				"binding/register_types.cpp"
				"d2/achievements/achievements.cpp"
				"d2/utilities/setup.cpp"
				"d2/utilities/utilities.cpp"
)

add_library(d2hub_backend SHARED ${SOURCE_FILES})

target_include_directories(d2hub_backend PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${godot-cpp_SOURCE_DIR}/include PUBLIC ${spdlog_SOURCE_DIR}/include PUBLIC ${game_enhancer_SOURCE_DIR}/include)
target_link_libraries(d2hub_backend PUBLIC godot-cpp game_enhancer spdlog::spdlog)

install(TARGETS d2hub_backend DESTINATION ${D2E_INSTALL_LIB_DIR})
install(FILES $<TARGET_FILE:d2hub_backend> DESTINATION ${D2E_FRONTEND_REQ_DIR})