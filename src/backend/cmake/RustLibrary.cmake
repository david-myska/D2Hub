
function(add_rust_library TARGET_NAME SOURCE_DIR)

    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(RUST_TARGET i686-pc-windows-msvc)
    else()
        set(RUST_TARGET x86_64-pc-windows-msvc)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(RUST_PROFILE release)
        set(RUST_FLAGS --release)
    else()
        set(RUST_PROFILE debug)
        set(RUST_FLAGS)
    endif()

    find_program(CARGO_EXECUTABLE cargo REQUIRED)

    set(CARGO_TARGET_DIR
        ${expro_BINARY_DIR}/${TARGET_NAME}
    )

    set(RUST_OUTPUT_DIR
        ${CARGO_TARGET_DIR}/${RUST_TARGET}/${RUST_PROFILE}
    )

    set(RUST_LIBRARY
        ${RUST_OUTPUT_DIR}/${TARGET_NAME}.dll)

    set(RUST_IMPLIB
        ${RUST_OUTPUT_DIR}/${TARGET_NAME}.dll.lib)


    file(GLOB_RECURSE RUST_SOURCES
        ${SOURCE_DIR}/src/*.rs
    )


    add_custom_command(
        OUTPUT
            ${RUST_LIBRARY}
            ${RUST_IMPLIB}

        COMMAND
            ${CMAKE_COMMAND} -E env
            CARGO_TARGET_DIR=${CARGO_TARGET_DIR}
            ${CARGO_EXECUTABLE} build
                ${RUST_FLAGS}
                --target ${RUST_TARGET}

        WORKING_DIRECTORY
            ${SOURCE_DIR}

        DEPENDS
            ${RUST_SOURCES}
            ${SOURCE_DIR}/Cargo.toml
            ${SOURCE_DIR}/Cargo.lock

        COMMENT
            "Building Rust library ${TARGET_NAME}"
    )


    add_custom_target(${TARGET_NAME}_build
        DEPENDS
            ${RUST_LIBRARY}
            ${RUST_IMPLIB}
    )


    add_library(${TARGET_NAME} SHARED IMPORTED GLOBAL)

    set_target_properties(${TARGET_NAME} PROPERTIES
        IMPORTED_LOCATION
            ${RUST_LIBRARY}

        IMPORTED_IMPLIB
            ${RUST_IMPLIB}

        INTERFACE_INCLUDE_DIRECTORIES
            ${SOURCE_DIR}/include
    )

endfunction()