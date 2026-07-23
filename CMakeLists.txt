cmake_minimum_required(VERSION 3.12)
project(CompilerDesignLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dynamically find ALL .cpp files recursively.
file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")

foreach(CPP_FILE_PATH IN LISTS CPP_FILES)

    # Ignore anything inside CLion's auto-generated build folders
    if(CPP_FILE_PATH MATCHES "cmake-build-")
        continue()
    endif()

    # Get folder path, folder name, and the file name without its extension
    get_filename_component(FOLDER_PATH "${CPP_FILE_PATH}" DIRECTORY)
    get_filename_component(FOLDER_NAME "${FOLDER_PATH}" NAME)
    get_filename_component(FILE_NAME_WE "${CPP_FILE_PATH}" NAME_WE)

    # 2. Create a unique target name (e.g., "task_2_main" or "assessment_1_q1")
    # This completely solves the issue of having multiple .cpp files in one folder.
    set(TARGET_NAME "${FOLDER_NAME}_${FILE_NAME_WE}")

    # 3. Create the executable
    add_executable(${TARGET_NAME} "${CPP_FILE_PATH}")

    # 4. Force the executable to be placed in its own distinct subfolder
    set(TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${FOLDER_NAME}/${FILE_NAME_WE}")
    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${TARGET_OUT_DIR}"
    )

    set(INPUT_FILE_PATH "${FOLDER_PATH}/input.txt")

    # 5. Check if an input.txt exists in the assignment folder
    if(EXISTS "${INPUT_FILE_PATH}")
        # Create a dedicated target that ALWAYS evaluates when you run the project.
        add_custom_target(${TARGET_NAME}_copy_input
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${INPUT_FILE_PATH}"
                "${TARGET_OUT_DIR}/input.txt"
        )

        # Make the executable strictly depend on the copy task so the text file syncs FIRST
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_copy_input)
    endif()

endforeach()