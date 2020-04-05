set(TARGET lab_01)
set(SOURCES
    utils.cc
    syntaxtree.cc
    dfa.cc)

add_library(${TARGET} ${SOURCES})
