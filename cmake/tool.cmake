##
## Copyright 2009-2019 Centreon
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

function(add_broker_module name activate)
  option(WITH_MODULE_${name} "Build ${name} module." ${activate})
  if (WITH_MODULE_${name})
    string(TOLOWER ${name} module_src_dir)
    add_subdirectory(${module_src_dir})
    set(MODULE_LIST ${MODULE_LIST} ${module_src_dir} PARENT_SCOPE)
    set(TESTS_SOURCES ${TESTS_SOURCES} PARENT_SCOPE)
    set(TESTS_LIBRARIES ${TESTS_LIBRARIES} PARENT_SCOPE)
  endif (WITH_MODULE_${name})
endfunction(add_broker_module name activate)