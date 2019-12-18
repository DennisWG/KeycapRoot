cmake_minimum_required(VERSION 3.7.0)

add_executable (test_${PROJECT_NAME}
    compression/zip.cpp
    configuration/config_file.cpp
    cryptography/ARC4.cpp
    cryptography/OTP.cpp
    network/srp6/srp6.cpp
    network/data_router.cpp
    network/memory_stream.cpp
    network/service.cpp
    network/service_locator.cpp
    utility/crc32.cpp
    utility/md5.cpp
    utility/enum.cpp
    utility/memory.cpp
    utility/random.cpp
    utility/utility.cpp
    main.cpp
)


add_sanitizers(test_${PROJECT_NAME})

target_link_libraries(test_${PROJECT_NAME} keycaproot rapidcheck ${Boost_LIBRARIES} ${Botan_LIBRARIES})

target_include_directories(test_${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/contrib/rapidcheck/include
        ${PROJECT_SOURCE_DIR}/contrib/rapidcheck/extras/catch/include
        ${PROJECT_SOURCE_DIR}/contrib/catch/single_include
)

add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME}
)

configure_file("configuration/test.cfg" "${CMAKE_CURRENT_BINARY_DIR}/test.cfg" @ONLY)
configure_file("configuration/invalid.cfg" "${CMAKE_CURRENT_BINARY_DIR}/invalid.cfg" @ONLY)

if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)
    set_target_properties(
            test_${PROJECT_NAME} PROPERTIES
            COMPILE_FLAGS "-O0 -g -fprofile-arcs -ftest-coverage"
            LINK_FLAGS "-O0 -g -lgcov -fprofile-arcs -ftest-coverage"
    )

    SETUP_TARGET_FOR_COVERAGE(
        coverage
        test_${PROJECT_NAME}
        coverage_report
    )
endif()
