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

add_library(keycaproot
    ${version_file}
    utility/crc32.cpp
    utility/random.cpp
    utility/utility.cpp
    "utility/string.cpp"
    "configuration/config_file.cpp"
    compression/zip.cpp
    cryptography/ARC4.cpp
    network/connection.cpp
    network/message_handler.cpp
    network/service_locator.cpp
    network/srp6/server.cpp
    network/srp6/group_parameters.cpp
    network/srp6/utility.cpp
)

target_include_directories(keycaproot
    PUBLIC
        ${PROJECT_SOURCE_DIR}/contrib/spdlog/include
        ${PROJECT_SOURCE_DIR}/contrib/gsl/include
        ${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/contrib/json/src
        ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries(keycaproot boost ${ZLIB_LIBRARIES})