cmake_minimum_required (VERSION 3.20)

project ("D2MXL_Enhanced")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(D2E_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(D2E_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(D2E_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(D2HUB_FRONTEND_REQ_DIR ${PROJECT_SOURCE_DIR}/src/frontend/bin)

add_subdirectory(src)

set(GODOT_EDITOR_PATH "" CACHE FILEPATH "Path to the Godot editor executable")

set(GODOT_PROJECT_PATH ${PROJECT_SOURCE_DIR}/src/frontend)
set(GODOT_EXPORT_PRESET "Windows")
set(GODOT_EXE_OUTPUT ${GODOT_PROJECT_PATH}/bin/D2Hub.exe)

add_custom_target(frontend_export
    COMMAND ${GODOT_EDITOR_PATH} --headless --path ${GODOT_PROJECT_PATH} --export-release "${GODOT_EXPORT_PRESET}" ${GODOT_EXE_OUTPUT}
    WORKING_DIRECTORY ${GODOT_PROJECT_PATH}
    COMMENT "Exporting Godot project to EXE"
    DEPENDS backend_copy
)

set(MANIFEST_FILE ${PROJECT_SOURCE_DIR}/admin.manifest)

add_custom_target(add_manifest
    COMMAND mt.exe -manifest ${MANIFEST_FILE} -outputresource:${GODOT_EXE_OUTPUT};1
    COMMENT "Embedding admin manifest into EXE"
    DEPENDS frontend_export
)

set(PACKAGE_OUTPUT ${PROJECT_SOURCE_DIR}/D2Hub.zip)

add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${PACKAGE_OUTPUT} --format=zip -- ${GODOT_PROJECT_PATH}/bin
    COMMENT "Zipping final output for distribution"
    DEPENDS add_manifest
)

add_custom_target(full
    DEPENDS backend_copy frontend_export add_manifest package
    COMMENT "Full build, export, manifest, and package"
)
