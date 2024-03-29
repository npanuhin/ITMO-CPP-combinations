project(combinations)

add_library(${PROJECT_NAME}
        include/combinations/Combinations.hpp src/Combinations.cpp
        include/combinations/Component.hpp src/Component.cpp
        include/combinations/Combination.hpp src/Combination.cpp
        include/combinations/DateTime.hpp src/DateTime.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(pugixml REQUIRED)

add_library(combinations::combinations ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC pugixml::pugixml)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(tests tests/load_test.cpp tests/test.cpp)
target_link_libraries(tests PRIVATE GTest::GTest combinations::combinations)
gtest_discover_tests(tests)

file(GLOB ETC_FILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/etc/*)

# Copy data files for tests
add_custom_command(
        OUTPUT ${ETC_FILES}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/etc ${CMAKE_CURRENT_BINARY_DIR}/test/etc
        DEPENDS ${ETC_FILES}
        COMMENT "Copying test data")

add_custom_target(
        etc DEPENDS ${ETC_FILES})

add_dependencies(tests etc)

if (COMPILE_OPTS)
    target_compile_options(${PROJECT_NAME} PUBLIC ${COMPILE_OPTS})
    target_link_options(${PROJECT_NAME} PUBLIC ${LINK_OPTS})

    target_compile_options(tests PUBLIC ${COMPILE_OPTS})
    target_link_options(tests PUBLIC ${LINK_OPTS})
endif ()
