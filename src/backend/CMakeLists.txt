
include(FetchContent)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH x86_32)
else()
    set(ARCH x86_64)
endif()

FetchContent_Declare(
  godot-cpp
  GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
  GIT_TAG        4.5
)
FetchContent_MakeAvailable(godot-cpp)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.3
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  game_ext_suite
  GIT_REPOSITORY https://github.com/david-myska/GameExtSuite.git
  GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(game_ext_suite)


set(SOURCE_FILES
				"binding/modules/module.cpp"
				"binding/modules/achievements_module.cpp"
				"binding/modules/backup_module.cpp"
				"binding/modules/bestiary_module.cpp"
				"binding/modules/developer_module.cpp"
				"binding/modules/lootfilter_module.cpp"
				"binding/modules/statistics_module.cpp"
				"binding/notifier.cpp"
				"binding/log_view.cpp"
				"binding/d2hub_backend.cpp"
				"binding/register_types.cpp"
				"d2/achievements/achievements.cpp"
				"d2/achievements/utilities.cpp"
				"d2/utilities/data.cpp"
				"d2/utilities/setup.cpp"
				"d2/utilities/utilities.cpp"
				"d2/utilities/loaded_data.cpp"
				# Achievements
				# Very Easy
				"d2/achievements/achievements/dungeons/very_easy/death_projector_high_health.cpp"
				# Easy
				"d2/achievements/achievements/dungeons/easy/binding_of_baal_kill_at_most_4_minions.cpp"
				"d2/achievements/achievements/dungeons/easy/akarat_let_heal_and_kill.cpp"
				"d2/achievements/achievements/dungeons/easy/horazons_sanctum_no_portal_kill_have_10_portals.cpp"
				"d2/achievements/achievements/rifts/easy/island_of_the_sunless_sea_gold_farm.cpp"
				"d2/achievements/achievements/rifts/easy/the_secret_cow_level_peaceful_gathering.cpp"
				# Moderate
				"d2/achievements/achievements/dungeons/moderate/heart_of_sin_negative_fire_res.cpp"
				"d2/achievements/achievements/dungeons/moderate/black_road_no_minion_in_circle.cpp"
				"d2/achievements/achievements/rifts/moderate/kurast_3000ba_kill_all_totems_timed.cpp"
				"d2/achievements/achievements/rifts/moderate/tran_athulua_kill_all_priestesses_simultaneously.cpp"
				# Hard
				"d2/achievements/achievements/dungeons/hard/toraja_max_5_edyrem.cpp"
				# Very Hard
				"d2/achievements/achievements/dungeons/very_hard/xazax_start_at_15_kill_before_30_pentients.cpp"
)

add_library(d2hub_backend SHARED ${SOURCE_FILES})

message(STATUS "GameExtSuite path: ${game_ext_suite_SOURCE_DIR}")
target_include_directories(d2hub_backend PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${godot-cpp_SOURCE_DIR}/include PUBLIC ${spdlog_SOURCE_DIR}/include)
target_link_libraries(d2hub_backend PUBLIC godot-cpp game_ext_suite spdlog::spdlog)

install(TARGETS d2hub_backend DESTINATION ${D2E_INSTALL_LIB_DIR})
install(FILES $<TARGET_FILE:d2hub_backend> DESTINATION ${D2E_FRONTEND_REQ_DIR})