
find_package (Criterion REQUIRED)
enable_testing()

set(TEST_SOURCE_FILES
        config_test.c
        error_test.c
        criterion_hooks.c)

add_executable(test-jsonfmt ${TEST_SOURCE_FILES})

include_directories(../src/lib ${CRITERION_INCLUDE_DIRS})

target_link_libraries(test-jsonfmt lib-jsonfmt ${CRITERION_LIBRARIES})

add_test(test-jsonfmt test-jsonfmt)
