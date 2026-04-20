cmake_minimum_required (VERSION 3.20)

project ("D2Hub")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(D2E_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(D2E_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(D2E_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(D2HUB_FRONTEND_BIN_DIR ${PROJECT_SOURCE_DIR}/src/frontend/bin)
set(BUILD_TOOLS_DIR ${PROJECT_SOURCE_DIR}/build/tools)

add_subdirectory(src)

set(GODOT_PROJECT_DIR ${PROJECT_SOURCE_DIR}/src/frontend)
set(GODOT_EXPORT_DIR ${PROJECT_SOURCE_DIR}/bin)
set(GODOT_EXPORT_PATH ${GODOT_EXPORT_DIR}/D2Hub.exe)
set(GODOT_EXPORT_PRESET "D2Hub export")

set(GODOT_EDITOR_VERSION "4.5.1" CACHE STRING "Godot editor version used for automated download")
set(GODOT_EDITOR_RELEASE "stable" CACHE STRING "Godot editor release channel used for automated download")
set(GODOT_EDITOR_ARCH "win32" CACHE STRING "Godot editor architecture suffix (win32, win64, windows_arm64)")
set(GODOT_EDITOR_DOWNLOAD_DIR "${BUILD_TOOLS_DIR}/godot" CACHE PATH "Directory where the downloaded Godot editor is stored")
set(GODOT_TEMPLATES_DOWNLOAD_DIR "${BUILD_TOOLS_DIR}/godot/export_templates" CACHE PATH "Directory where Godot export templates are cached")
set(GODOT_DOWNLOADED_EDITOR_PATH "${GODOT_EDITOR_DOWNLOAD_DIR}/godot_v${GODOT_EDITOR_VERSION}-${GODOT_EDITOR_RELEASE}_${GODOT_EDITOR_ARCH}.exe")

set(GODOT_EDITOR_PATH "${GODOT_DOWNLOADED_EDITOR_PATH}" CACHE FILEPATH "Path to the Godot editor executable")
set(GODOT_CONSOLE_EDITOR_PATH "${GODOT_EDITOR_DOWNLOAD_DIR}/godot_v${GODOT_EDITOR_VERSION}-${GODOT_EDITOR_RELEASE}_${GODOT_EDITOR_ARCH}_console.exe")
set(GODOT_EXPORT_RUNNER_PATH "${GODOT_EDITOR_PATH}")
if(GODOT_EDITOR_PATH STREQUAL GODOT_DOWNLOADED_EDITOR_PATH AND EXISTS "${GODOT_CONSOLE_EDITOR_PATH}")
    set(GODOT_EXPORT_RUNNER_PATH "${GODOT_CONSOLE_EDITOR_PATH}")
endif()

add_custom_target(download_godot_editor
    COMMAND ${CMAKE_COMMAND}
        "-DGODOT_VERSION=${GODOT_EDITOR_VERSION}"
        "-DGODOT_RELEASE=${GODOT_EDITOR_RELEASE}"
        "-DGODOT_ARCH=${GODOT_EDITOR_ARCH}"
        "-DGODOT_DOWNLOAD_DIR=${GODOT_EDITOR_DOWNLOAD_DIR}"
        -P ${PROJECT_SOURCE_DIR}/cmake/DownloadGodotEditor.cmake
    COMMENT "Downloading Godot editor ${GODOT_EDITOR_VERSION}-${GODOT_EDITOR_RELEASE} (${GODOT_EDITOR_ARCH})"
)

add_custom_target(download_godot_templates
    COMMAND ${CMAKE_COMMAND}
        "-DGODOT_VERSION=${GODOT_EDITOR_VERSION}"
        "-DGODOT_RELEASE=${GODOT_EDITOR_RELEASE}"
        "-DGODOT_TEMPLATES_DIR=${GODOT_TEMPLATES_DOWNLOAD_DIR}"
        -P ${PROJECT_SOURCE_DIR}/cmake/DownloadGodotTemplates.cmake
    COMMENT "Downloading Godot export templates ${GODOT_EDITOR_VERSION}-${GODOT_EDITOR_RELEASE}"
)

set(FRONTEND_EXPORT_DEPENDENCIES backend_copy)
if(GODOT_EDITOR_PATH STREQUAL GODOT_DOWNLOADED_EDITOR_PATH)
    list(APPEND FRONTEND_EXPORT_DEPENDENCIES download_godot_editor)
    list(APPEND FRONTEND_EXPORT_DEPENDENCIES download_godot_templates)
else()
    message(STATUS "Using custom Godot editor path; automatic editor download skipped")
endif()

add_custom_target(frontend_export
    COMMAND ${CMAKE_COMMAND}
        "-DASSERT_PATH=${GODOT_EDITOR_PATH}"
        "-DASSERT_LABEL=Godot editor"
        -P ${PROJECT_SOURCE_DIR}/cmake/AssertFileExists.cmake
    COMMAND ${CMAKE_COMMAND} -E env "APPDATA=${BUILD_TOOLS_DIR}" "${GODOT_EXPORT_RUNNER_PATH}" --headless --path ${GODOT_PROJECT_DIR} --export-release "${GODOT_EXPORT_PRESET}" ${GODOT_EXPORT_PATH}
    COMMAND ${CMAKE_COMMAND}
        "-DASSERT_PATH=${GODOT_EXPORT_PATH}"
        "-DASSERT_LABEL=Exported binary"
        -P ${PROJECT_SOURCE_DIR}/cmake/AssertFileExists.cmake
    WORKING_DIRECTORY ${GODOT_PROJECT_DIR}
    COMMENT "Exporting Godot project to EXE"
    DEPENDS ${FRONTEND_EXPORT_DEPENDENCIES}
)

set(MANIFEST_FILE ${PROJECT_SOURCE_DIR}/admin.manifest)

add_custom_target(add_manifest
    COMMAND mt.exe -manifest ${MANIFEST_FILE} -outputresource:${GODOT_EXPORT_PATH}\\;1
    COMMENT "Embedding admin manifest into EXE"
    DEPENDS frontend_export
)

set(PACKAGE_OUTPUT ${GODOT_EXPORT_DIR}/D2Hub.zip)
set(PACKAGE_DIR ${GODOT_EXPORT_DIR}/tmp_package)

# to make the zip content flat with no weird subdirectories
add_custom_target(package_prep
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}

    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_EXPORT_PATH} ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_EXPORT_DIR}/d2hub_backend.dll ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_PROJECT_DIR}/stat_ids.txt ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_PROJECT_DIR}/custom_stat_ids.txt ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_PROJECT_DIR}/item_ids.txt ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_PROJECT_DIR}/custom_item_ids.txt ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${GODOT_PROJECT_DIR}/custom_minion_ids.txt ${PACKAGE_DIR}

    COMMENT "Preparing package directory"
    DEPENDS add_manifest
)

add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${PACKAGE_OUTPUT} --format=zip -- .
    WORKING_DIRECTORY ${PACKAGE_DIR}
    COMMENT "Zipping final output for distribution"
    DEPENDS package_prep
)

add_custom_target(full
    DEPENDS backend_copy frontend_export add_manifest package
    COMMENT "Full build, export, manifest, and package"
)
