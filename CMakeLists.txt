cmake_minimum_required(VERSION 3.12)
project(CompilerDesignLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dynamically find all main.cpp files.
file(GLOB_RECURSE MAIN_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*/main.cpp")

foreach(MAIN_CPP_PATH IN LISTS MAIN_FILES)

    # Ignore anything inside CLion's auto-generated build folders
    if(MAIN_CPP_PATH MATCHES "cmake-build-")
        continue()
    endif()

    get_filename_component(FOLDER_PATH "${MAIN_CPP_PATH}" DIRECTORY)
    get_filename_component(TARGET_NAME "${FOLDER_PATH}" NAME)

    # 2. Create the executable
    add_executable(${TARGET_NAME} "${MAIN_CPP_PATH}")

    # 3. Force the executable to be placed in its own subfolder inside the debug directory.
    # This prevents overlap and completely breaks the cyclic dependency loop!
    set(TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}")
    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${TARGET_OUT_DIR}"
    )

    set(INPUT_FILE_PATH "${FOLDER_PATH}/input.txt")

    # 4. Check if an input.txt exists in the assignment folder
    if(EXISTS "${INPUT_FILE_PATH}")
        # Create a dedicated target that ALWAYS evaluates when you run the project.
        # No generator expressions = No infinite loops.
        add_custom_target(${TARGET_NAME}_copy_input
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${INPUT_FILE_PATH}"
                "${TARGET_OUT_DIR}/input.txt"
        )

        # Make the executable strictly depend on the copy task so the text file syncs FIRST
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_copy_input)
    endif()

endforeach()