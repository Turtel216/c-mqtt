file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.c)

add_executable(mqtt_tests ${TEST_SOURCES})
target_include_directories(mqtt_tests PRIVATE ../src)
target_link_libraries(mqtt_tests PRIVATE broker_lib)

add_test(NAME MQTTTests COMMAND mqtt_tests)
