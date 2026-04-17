cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED ASSERT_PATH OR ASSERT_PATH STREQUAL "")
    message(FATAL_ERROR "ASSERT_PATH must be provided")
endif()

if(NOT DEFINED ASSERT_LABEL OR ASSERT_LABEL STREQUAL "")
    set(ASSERT_LABEL "File")
endif()

if(NOT EXISTS "${ASSERT_PATH}")
    message(FATAL_ERROR "${ASSERT_LABEL} not found at '${ASSERT_PATH}'")
endif()
