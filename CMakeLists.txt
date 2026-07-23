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

    # 2. Create a unique target name (e.g., "task_2_main" or "vtop_assessment_1_symbolTable")
    set(TARGET_NAME "${FOLDER_NAME}_${FILE_NAME_WE}")

    # 3. Create the executable
    add_executable(${TARGET_NAME} "${CPP_FILE_PATH}")

    # 4. Force the executable to be placed in its own distinct subfolder
    set(TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${FOLDER_NAME}/${FILE_NAME_WE}")
    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${TARGET_OUT_DIR}"
    )

    # 5. Dynamically find ALL .txt files in the same directory as the .cpp file
    file(GLOB TXT_FILES "${FOLDER_PATH}/*.txt")

    # 6. Copy all found .txt files to the executable's output directory safely
    if(TXT_FILES)
        foreach(TXT_FILE IN LISTS TXT_FILES)
            get_filename_component(TXT_FILE_NAME "${TXT_FILE}" NAME)

            # Define the destination path
            set(TXT_FILE_DEST "${TARGET_OUT_DIR}/${TXT_FILE_NAME}")

            # Create a custom command that tells CMake HOW to generate the destination file.
            # It will automatically re-run if the DEPENDS file (your original .txt) is modified.
            add_custom_command(
                    OUTPUT "${TXT_FILE_DEST}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${TXT_FILE}"
                    "${TXT_FILE_DEST}"
                    DEPENDS "${TXT_FILE}"
                    COMMENT "Copying/Updating ${TXT_FILE_NAME} for ${TARGET_NAME}"
            )

            # Add the destination file as a source to the executable target.
            # This forces the build system to evaluate the dependency chain and
            # execute the copy command before running the executable, even if
            # the .cpp file hasn't changed.
            target_sources(${TARGET_NAME} PRIVATE "${TXT_FILE_DEST}")

        endforeach()
    endif()

endforeach()