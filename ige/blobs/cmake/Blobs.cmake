function(write_blob_header DEST BLOBNAME)
    # symbol name for the include guard
    set(GUARD_SYM "BLOB_${BLOBNAME}_H")
    set(SRC "")

    string(APPEND SRC "#ifndef ${GUARD_SYM}\n")
    string(APPEND SRC "#define ${GUARD_SYM}\n")
    string(APPEND SRC "#ifdef __cplusplus\n")
    string(APPEND SRC "extern \"C\" {\n")
    string(APPEND SRC "#endif\n")
    string(APPEND SRC "#include <stddef.h>\n")
    string(APPEND SRC "extern const size_t ${BLOBNAME}_SIZE\;\n")
    string(APPEND SRC "extern const char ${BLOBNAME}[]\;\n")
    string(APPEND SRC "#ifdef __cplusplus\n")
    string(APPEND SRC "}\n")
    string(APPEND SRC "#endif\n")
    string(APPEND SRC "#endif\n")

    set(CURRENT_SRC "")

	if(EXISTS ${DEST})
        file(READ ${DEST} CURRENT_SRC)
    endif()

    if(NOT "${CURRENT_SRC}" STREQUAL "${SRC}")
        file(WRITE ${DEST} ${SRC})
    endif()
endfunction()

function(write_blob_source DEST BLOBNAME HEADER FILEPATH)
    file(SIZE ${FILEPATH} FILESIZE)
    file(READ ${FILEPATH} FILEDATA HEX)

    string(APPEND FILEDATA "00")

    string(REPEAT "[0-9a-f]" 16 ROW_REGEX)
    string(REGEX REPLACE "(${ROW_REGEX})" "\\1\n    " FILEDATA ${FILEDATA})
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," FILEDATA ${FILEDATA})
    string(STRIP ${FILEDATA} FILEDATA)
    string(PREPEND FILEDATA "    ")

    file(WRITE ${DEST} "#include \"${HEADER}\"\n")
    file(APPEND ${DEST} "#include <stddef.h>\n")
    file(APPEND ${DEST} "const size_t ${BLOBNAME}_SIZE = ${FILESIZE};\n")
    file(APPEND ${DEST} "const char ${BLOBNAME}[] = {\n")
    file(APPEND ${DEST} "${FILEDATA}\n")
    file(APPEND ${DEST} "};\n")
endfunction()

function(add_blobs TARGET_NAME)
    set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}")
    set(BLOB_SOURCES "")

    foreach(BLOBPATH ${ARGN})
        set(OUTPUT_C "${OUTPUT_DIR}/src/${BLOBPATH}.c")
        set(OUTPUT_H "${OUTPUT_DIR}/include/${TARGET_NAME}/${BLOBPATH}.h")

        set(FULLBLOBPATH "${CMAKE_CURRENT_SOURCE_DIR}/${BLOBPATH}")

        set_property(DIRECTORY APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS ${FULLBLOBPATH})
        source_group(
            TREE "${CMAKE_CURRENT_SOURCE_DIR}"
            PREFIX "Blobs"
            FILES ${FULLBLOBPATH}
        )

        if(NOT EXISTS ${FULLBLOBPATH})
            message(FATAL_ERROR "File \"${FULLBLOBPATH}\" does not exist.")
        endif()

        string(MAKE_C_IDENTIFIER "${TARGET_NAME}/${BLOBPATH}" BLOBNAME)
        string(TOUPPER ${BLOBNAME} BLOBNAME)

        write_blob_header(${OUTPUT_H} ${BLOBNAME})

        if(${FULLBLOBPATH} IS_NEWER_THAN ${OUTPUT_C})
            write_blob_source(${OUTPUT_C} ${BLOBNAME}
                "${TARGET_NAME}/${BLOBPATH}.h" ${FULLBLOBPATH})
        endif()

        set(BLOB_SOURCES ${BLOB_SOURCES} ${OUTPUT_C} ${OUTPUT_H})
    endforeach()

    add_library(${TARGET_NAME} STATIC ${BLOB_SOURCES})
    target_include_directories(${TARGET_NAME} PUBLIC "${OUTPUT_DIR}/include")
endfunction()
