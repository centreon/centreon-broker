##
## Copyright 2014-2015 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/notification/inc/com/centreon/broker/notification")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/notification/src")
include_directories("${PROJECT_SOURCE_DIR}/notification/inc")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/file/inc")
include_directories("${PROJECT_SOURCE_DIR}/bbdo/inc")
include_directories("${PROJECT_SOURCE_DIR}/compression/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/process_manager.hh"
  "${INC_DIR}/process.hh")

# Notification module.
set(NOTIFICATION "40-notification")
set(NOTIFICATION "${NOTIFICATION}" PARENT_SCOPE)
add_library("${NOTIFICATION}" SHARED
  ${QT_WRAPPED_SOURCES}
  # Sources.
  "${SRC_DIR}/action.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/macro_context.cc"
  "${SRC_DIR}/macro_generator.cc"
  "${SRC_DIR}/macro_getters.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/node_cache.cc"
  "${SRC_DIR}/notification_scheduler.cc"
  "${SRC_DIR}/process.cc"
  "${SRC_DIR}/process_manager.cc"
  "${SRC_DIR}/run_queue.cc"
  "${SRC_DIR}/state.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/objects/command.cc"
  "${SRC_DIR}/objects/contact.cc"
  "${SRC_DIR}/objects/dependency.cc"
  "${SRC_DIR}/objects/node.cc"
  "${SRC_DIR}/objects/node_id.cc"
  "${SRC_DIR}/objects/defines.cc"
  "${SRC_DIR}/objects/notification_method.cc"
  "${SRC_DIR}/objects/notification_rule.cc"
  "${SRC_DIR}/loaders/contact_loader.cc"
  "${SRC_DIR}/loaders/dependency_loader.cc"
  "${SRC_DIR}/loaders/node_loader.cc"
  "${SRC_DIR}/loaders/timeperiod_loader.cc"
  "${SRC_DIR}/loaders/command_loader.cc"
  "${SRC_DIR}/loaders/notification_method_loader.cc"
  "${SRC_DIR}/loaders/notification_rule_loader.cc"
  "${SRC_DIR}/loaders/macro_loader.cc"
  "${SRC_DIR}/builders/node_set_builder.cc"
  "${SRC_DIR}/builders/composed_contact_builder.cc"
  "${SRC_DIR}/builders/contact_by_id_builder.cc"
  "${SRC_DIR}/builders/composed_dependency_builder.cc"
  "${SRC_DIR}/builders/composed_node_builder.cc"
  "${SRC_DIR}/builders/composed_timeperiod_builder.cc"
  "${SRC_DIR}/builders/composed_command_builder.cc"
  "${SRC_DIR}/builders/composed_notification_method_builder.cc"
  "${SRC_DIR}/builders/composed_notification_rule_builder.cc"
  "${SRC_DIR}/builders/dependency_by_node_id_builder.cc"
  "${SRC_DIR}/builders/timeperiod_by_id_builder.cc"
  "${SRC_DIR}/builders/timeperiod_linker.cc"
  "${SRC_DIR}/builders/command_by_id_builder.cc"
  "${SRC_DIR}/builders/node_by_node_id_builder.cc"
  "${SRC_DIR}/builders/global_macro_builder.cc"
  "${SRC_DIR}/builders/notification_method_by_id_builder.cc"
  "${SRC_DIR}/builders/notification_rule_by_node_builder.cc"
  "${SRC_DIR}/builders/notification_rule_by_id_builder.cc"
  "${SRC_DIR}/utilities/data_loggers.cc"
  "${SRC_DIR}/utilities/qhash_func.cc"
  "${SRC_DIR}/utilities/get_datetime_string.cc"
  # Headers.
  "${INC_DIR}/action.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/macro_context.hh"
  "${INC_DIR}/macro_generator.hh"
  "${INC_DIR}/macro_getters.hh"
  "${INC_DIR}/node_cache.hh"
  "${INC_DIR}/notification_scheduler.hh"
  "${INC_DIR}/process.hh"
  "${INC_DIR}/process_manager.hh"
  "${INC_DIR}/run_queue.hh"
  "${INC_DIR}/state.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/objects/command.hh"
  "${INC_DIR}/objects/contact.hh"
  "${INC_DIR}/objects/dependency.hh"
  "${INC_DIR}/objects/node.hh"
  "${INC_DIR}/objects/node_id.hh"
  "${INC_DIR}/objects/defines.hh"
  "${INC_DIR}/objects/notification_method.hh"
  "${INC_DIR}/objects/notification_rule.hh"
  "${INC_DIR}/loaders/command_loader.hh"
  "${INC_DIR}/loaders/contact_loader.hh"
  "${INC_DIR}/loaders/dependency_loader.hh"
  "${INC_DIR}/loaders/node_loader.hh"
  "${INC_DIR}/loaders/timeperiod_loader.hh"
  "${INC_DIR}/loaders/notification_method_loader.hh"
  "${INC_DIR}/loaders/notification_rule_loader.hh"
  "${INC_DIR}/loaders/macro_loader.hh"
  "${INC_DIR}/builders/timeperiod_builder.hh"
  "${INC_DIR}/builders/command_builder.hh"
  "${INC_DIR}/builders/contact_builder.hh"
  "${INC_DIR}/builders/dependency_builder.hh"
  "${INC_DIR}/builders/node_builder.hh"
  "${INC_DIR}/builders/node_set_builder.hh"
  "${INC_DIR}/builders/composed_contact_builder.hh"
  "${INC_DIR}/builders/contact_by_id_builder.hh"
  "${INC_DIR}/builders/composed_builder.hh"
  "${INC_DIR}/builders/composed_dependency_builder.hh"
  "${INC_DIR}/builders/composed_node_builder.hh"
  "${INC_DIR}/builders/composed_timeperiod_builder.hh"
  "${INC_DIR}/builders/composed_command_builder.hh"
  "${INC_DIR}/builders/composed_notification_method_builder.hh"
  "${INC_DIR}/builders/composed_notification_rule_builder.hh"
  "${INC_DIR}/builders/dependency_by_node_id_builder.hh"
  "${INC_DIR}/builders/timeperiod_by_id_builder.hh"
  "${INC_DIR}/builders/timeperiod_linker.hh"
  "${INC_DIR}/builders/command_by_id_builder.hh"
  "${INC_DIR}/builders/macro_builder.hh"
  "${INC_DIR}/builders/global_macro_builder.hh"
  "${INC_DIR}/builders/node_by_node_id_builder.hh"
  "${INC_DIR}/builders/notification_method_builder.hh"
  "${INC_DIR}/builders/notification_method_by_id_builder.hh"
  "${INC_DIR}/builders/notification_rule_builder.hh"
  "${INC_DIR}/builders/notification_rule_by_node_builder.hh"
  "${INC_DIR}/builders/notification_rule_by_id_builder.hh"
  "${INC_DIR}/utilities/ptr_typedef.hh"
  "${INC_DIR}/utilities/data_loggers.hh"
  "${INC_DIR}/utilities/qhash_func.hh"
  "${INC_DIR}/utilities/get_datetime_string.hh"
)
set_target_properties("${NOTIFICATION}" PROPERTIES
    PREFIX "")
target_link_libraries("${NOTIFICATION}" ${QT_LIBRARIES})

# Install rule.
install(TARGETS "${NOTIFICATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
