find_package(Qt6 REQUIRED COMPONENTS Test)

function( add_qcc_db_test TEST_NAME )
    add_executable( ${TEST_NAME} )

    target_sources( ${TEST_NAME}
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/${TEST_NAME}.cpp
            ${CMAKE_CURRENT_LIST_DIR}/${TEST_NAME}.h
    )

    target_link_libraries( ${TEST_NAME}
        PRIVATE
            QCC_DB_LIB
            Qt6::Test
    )

    if ( WIN32 )
        set_target_properties( ${TEST_NAME} PROPERTIES
            WIN32_EXECUTABLE False
        )
    endif()

    add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endfunction()

add_qcc_db_test( TestCCNormalCompressor )
add_qcc_db_test( TestCCColorScale )
add_qcc_db_test( TestCCScalarField )
