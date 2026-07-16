cmake_minimum_required(VERSION 3.12)
project(CompilerDesignLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find all main.cpp files dynamically in all subfolders.
# CONFIGURE_DEPENDS forces CMake to refresh when you create a new folder/file.
file(GLOB_RECURSE MAIN_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*/main.cpp")

# 2. Loop through every main.cpp found
foreach(MAIN_CPP_PATH IN LISTS MAIN_FILES)

    # Skip any files that accidentally get swept up in CLion's build directories
    if(MAIN_CPP_PATH MATCHES "cmake-build-")
        continue()
    endif()

    # Extract the absolute path of the folder and the folder's actual name
    get_filename_component(FOLDER_PATH "${MAIN_CPP_PATH}" DIRECTORY)
    get_filename_component(TARGET_NAME "${FOLDER_PATH}" NAME)

    # 3. Create the executable named exactly after the folder (e.g., "task_2")
    add_executable(${TARGET_NAME} "${MAIN_CPP_PATH}")

    # 4. Dynamically handle input.txt
    # If input.txt exists in this specific assignment folder, copy it at build time
    set(INPUT_FILE_PATH "${FOLDER_PATH}/input.txt")

    if(EXISTS "${INPUT_FILE_PATH}")
        # POST_BUILD ensures that if you modify input.txt and hit Run,
        # the fresh version gets copied over to the executable's directory.
        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${INPUT_FILE_PATH}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/input.txt"
        )
    endif()

endforeach()