##
## Copyright 2021 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
##

# Global options.
set(SRC_DIR "${PROJECT_SOURCE_DIR}/examples")

add_executable(broker-log-debug rpc/client.cc)
target_link_libraries(broker-log-debug berpc ${gRPC_LIBS} ${absl_LIBS} ${OpenSSL_LIBS} ${c-ares_LIBS} ${ZLIB_LIBS} dl pthread)
