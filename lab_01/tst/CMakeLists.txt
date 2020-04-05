set(TESTS
    syntaxtree.cc
    dfa.cc)

foreach(target ${TESTS})
        get_filename_component(TARGET ${target} NAME_WE)
        add_executable(${TARGET} ${target})
        add_test(${TARGET} ${TARGET})
        target_link_libraries(${TARGET}
            lab_01
                GTest::GTest
                GMock::GMock)
        target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src)
endforeach(target)
