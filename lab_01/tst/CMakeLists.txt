set(TESTS
    utils.cc
    syntaxtree.cc
    dfa.cc)

foreach(target ${TESTS})
        get_filename_component(TARGET ${target} NAME_WE)
        add_executable(${TARGET} ${target})
        add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
        target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src)
        target_link_libraries(${TARGET}
            lab_01
                GTest::GTest
                GMock::GMock)
endforeach(target)
