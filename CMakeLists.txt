cmake_minimum_required (VERSION 3.20)

project ("D2Hub")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(D2E_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(D2E_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(D2E_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(D2HUB_FRONTEND_BIN_DIR ${PROJECT_SOURCE_DIR}/src/frontend/bin)

add_subdirectory(src)

set(GODOT_EDITOR_PATH "" CACHE FILEPATH "Path to the Godot editor executable")

set(GODOT_PROJECT_DIR ${PROJECT_SOURCE_DIR}/src/frontend)
set(GODOT_EXPORT_DIR ${PROJECT_SOURCE_DIR}/bin)
set(GODOT_EXPORT_PATH ${GODOT_EXPORT_DIR}/D2Hub.exe)
set(GODOT_EXPORT_PRESET "D2Hub export")

add_custom_target(frontend_export
    COMMAND ${GODOT_EDITOR_PATH} --headless --path ${GODOT_PROJECT_DIR} --export-release "${GODOT_EXPORT_PRESET}" ${GODOT_EXPORT_PATH}
    WORKING_DIRECTORY ${GODOT_PROJECT_DIR}
    COMMENT "Exporting Godot project to EXE"
    DEPENDS backend_copy
)

set(MANIFEST_FILE ${PROJECT_SOURCE_DIR}/admin.manifest)

add_custom_target(add_manifest
    COMMAND mt.exe -manifest ${MANIFEST_FILE} -outputresource:${GODOT_EXPORT_PATH};1
    COMMENT "Embedding admin manifest into EXE"
    DEPENDS frontend_export
)

set(PACKAGE_OUTPUT ${GODOT_EXPORT_DIR}/D2Hub.zip)

add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${PACKAGE_OUTPUT} --format=zip -- ${GODOT_EXPORT_DIR}
    COMMENT "Zipping final output for distribution"
    DEPENDS add_manifest
)

add_custom_target(full
    DEPENDS backend_copy frontend_export add_manifest package
    COMMENT "Full build, export, manifest, and package"
)
