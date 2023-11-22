
set(SOURCES
        config.c
        error.c
        array.c
)

add_library (lib-jsonfmt STATIC ${SOURCES})
