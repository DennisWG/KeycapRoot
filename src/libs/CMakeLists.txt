#   Copyright 2017 KeycapEmu
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
cmake_minimum_required(VERSION 3.26)

find_package(Botan 3.1.1 REQUIRED)

botan_generate(botan_lib
    base32
    hex
    numbertheory
    md5
    sha1
    sha2_64
    hmac
    hmac_drbg
    auto_rng
    bigint
    system_rng 
)

add_library(keycaproot
    ${version_file}
    utility/crc32.cpp
    utility/md5.cpp
    utility/random.cpp
    utility/utility.cpp
    "utility/string.cpp"
    "configuration/config_file.cpp"
    compression/zip.cpp
    cryptography/ARC4.cpp
    cryptography/OTP.cpp
    network/connection.cpp
    network/data_router.cpp
    network/memory_stream.cpp
    network/message_handler.cpp
    network/service_base.cpp
    network/service_locator.cpp
    network/srp6/client.cpp
    network/srp6/server.cpp
    network/srp6/group_parameters.cpp
    network/srp6/utility.cpp
)

target_include_directories(keycaproot
    PRIVATE
        ${PROJECT_SOURCE_DIR}/contrib/gsl/include
    PUBLIC
        ${PROJECT_SOURCE_DIR}/contrib/json/include
        ${PROJECT_SOURCE_DIR}/contrib/spdlog/include
        ${PROJECT_SOURCE_DIR}/src/include
)

target_link_libraries(keycaproot
    PRIVATE
        keycap::root::project_options
        keycap::root::project_warnings

        botan_lib
        Boost::system
        Boost::lexical_cast
        zlibstatic

    PUBLIC
        Boost::asio
        Boost::crc
        Boost::uuid
)