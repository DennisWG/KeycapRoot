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

set(LIBRARY_NAME keycaproot)

set(UTILITY_SRC
    ${version_file}
    Utility/Utility.cpp
)

set(TEST_SRC
    Test.cpp
)

set(LIBRARY_SRC
    ${UTILITY_SRC}
    ${CONFIG_SRC}
    ${TEST_SRC}
)

source_group("Version" FILES ${VERSION_SRC})
source_group("Utility" FILES ${UTILITY_SRC})

add_library(${LIBRARY_NAME} ${LIBRARY_SRC})
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES})