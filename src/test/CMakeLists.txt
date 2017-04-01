cmake_minimum_required(VERSION 3.0.0)

include_directories ("${PROJECT_SOURCE_DIR}/contrib/rapidcheck/include")
include_directories ("${PROJECT_SOURCE_DIR}/contrib/rapidcheck/extras/catch/include")
include_directories ("${PROJECT_SOURCE_DIR}/contrib/catch/include")

file(GLOB tests
    "Configuration/*.cpp"
    "Network/*.cpp"
    "Utility/*.cpp"
    "*.cpp"
)

add_executable (test_${PROJECT_NAME} ${tests})
target_link_libraries(test_${PROJECT_NAME} keycaproot rapidcheck)


add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME}
)

configure_file("Configuration/test.cfg" "${CMAKE_CURRENT_BINARY_DIR}/test.cfg" @ONLY)
configure_file("Configuration/invalid.cfg" "${CMAKE_CURRENT_BINARY_DIR}/invalid.cfg" @ONLY)