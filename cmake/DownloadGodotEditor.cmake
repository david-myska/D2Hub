cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED GODOT_VERSION OR GODOT_VERSION STREQUAL "")
    message(FATAL_ERROR "GODOT_VERSION must be provided")
endif()

if(NOT DEFINED GODOT_RELEASE OR GODOT_RELEASE STREQUAL "")
    set(GODOT_RELEASE "stable")
endif()

if(NOT DEFINED GODOT_ARCH OR GODOT_ARCH STREQUAL "")
    set(GODOT_ARCH "win32")
endif()

if(NOT DEFINED GODOT_DOWNLOAD_DIR OR GODOT_DOWNLOAD_DIR STREQUAL "")
    message(FATAL_ERROR "GODOT_DOWNLOAD_DIR must be provided")
endif()

set(GODOT_ARCHIVE_NAME "Godot_v${GODOT_VERSION}-${GODOT_RELEASE}_${GODOT_ARCH}.exe.zip")
set(GODOT_EDITOR_NAME "Godot_v${GODOT_VERSION}-${GODOT_RELEASE}_${GODOT_ARCH}.exe")
set(GODOT_URL "https://github.com/godotengine/godot/releases/download/${GODOT_VERSION}-${GODOT_RELEASE}/${GODOT_ARCHIVE_NAME}")
set(GODOT_ARCHIVE_PATH "${GODOT_DOWNLOAD_DIR}/${GODOT_ARCHIVE_NAME}")
set(GODOT_EDITOR_PATH "${GODOT_DOWNLOAD_DIR}/${GODOT_EDITOR_NAME}")

file(MAKE_DIRECTORY "${GODOT_DOWNLOAD_DIR}")

if(EXISTS "${GODOT_EDITOR_PATH}")
    message(STATUS "Godot editor already present at ${GODOT_EDITOR_PATH}")
    return()
endif()

if(NOT EXISTS "${GODOT_ARCHIVE_PATH}")
    message(STATUS "Downloading Godot editor from ${GODOT_URL}")
    file(DOWNLOAD "${GODOT_URL}" "${GODOT_ARCHIVE_PATH}" STATUS DOWNLOAD_STATUS SHOW_PROGRESS)
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)
    list(GET DOWNLOAD_STATUS 1 DOWNLOAD_MESSAGE)
    if(NOT DOWNLOAD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to download Godot editor: ${DOWNLOAD_MESSAGE}")
    endif()
endif()

message(STATUS "Extracting ${GODOT_ARCHIVE_NAME}")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xvf "${GODOT_ARCHIVE_NAME}"
    WORKING_DIRECTORY "${GODOT_DOWNLOAD_DIR}"
    RESULT_VARIABLE EXTRACT_RESULT
)
if(NOT EXTRACT_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to extract Godot archive ${GODOT_ARCHIVE_PATH}")
endif()

if(NOT EXISTS "${GODOT_EDITOR_PATH}")
    message(FATAL_ERROR "Expected extracted editor not found at ${GODOT_EDITOR_PATH}")
endif()

message(STATUS "Godot editor is ready at ${GODOT_EDITOR_PATH}")
