cmake_minimum_required(VERSION 3.12)
project(CompilerDesignLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dynamically find all main.cpp files.
# CONFIGURE_DEPENDS updates automatically when new folders are added.
file(GLOB_RECURSE MAIN_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*/main.cpp")

foreach(MAIN_CPP_PATH IN LISTS MAIN_FILES)

    # Ignore anything inside CLion's auto-generated build folders
    if(MAIN_CPP_PATH MATCHES "cmake-build-")
        continue()
    endif()

    get_filename_component(FOLDER_PATH "${MAIN_CPP_PATH}" DIRECTORY)
    get_filename_component(TARGET_NAME "${FOLDER_PATH}" NAME)

    set(INPUT_FILE_PATH "${FOLDER_PATH}/input.txt")

    # 2. Check if an input.txt exists in the assignment folder
    if(EXISTS "${INPUT_FILE_PATH}")
        # Add input.txt AS A SOURCE. This makes CMake track modifications to it.
        # When you edit the text file, it triggers the POST_BUILD command automatically.
        add_executable(${TARGET_NAME} "${MAIN_CPP_PATH}" "${INPUT_FILE_PATH}")

        # Simply copy it over after the build finishes
        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${INPUT_FILE_PATH}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/input.txt"
        )
    else()
        # If there's no input.txt, just compile normally
        add_executable(${TARGET_NAME} "${MAIN_CPP_PATH}")
    endif()

endforeach()