macro(register_source_groups_from_filestructure list_of_files remove_prefix)
    foreach(FILE ${list_of_files})
        get_filename_component(PARENT_DIR "${FILE}" PATH)

        # skip src or include and changes /'s to \\'s
        string(REPLACE ${PROJECT_SOURCE_DIR} "" PARENT_DIR "${PARENT_DIR}")
        string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
        string(REGEX REPLACE "${remove_prefix}/?" "" GROUP "${GROUP}")
               #string(REGEX REPLACE "/storm?/" "" GROUP ${GROUP})
        #STRING(SUBSTRING "${GROUP}" 1 -1 GROUP)
         string(REPLACE "/" "\\" GROUP "${GROUP}")
		
        # group into "Source Files" and "Header Files"
        # if ("${FILE}" MATCHES ".*\\.cpp")
        #   set(GROUP "Source Files\\${GROUP}")
        # elseif("${FILE}" MATCHES ".*\\.h")
        #   set(GROUP "Header Files\\${GROUP}")
        # endif()

        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endmacro()