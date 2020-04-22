set(TARGET lab_02)
set(SOURCES
    utils.cc
    leftutils.cc
    chomskyutils.cc
)

add_library(${TARGET} ${SOURCES})
