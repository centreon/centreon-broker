/*
** Copyright 2009-2013,2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NEB_CALLBACKS_HH
#define CCB_NEB_CALLBACKS_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
extern int gl_mod_flags;
extern void* gl_mod_handle;

int callback_acknowledgement(int callback_type, void* data);
int callback_comment(int callback_type, void* data);
int callback_custom_variable(int callback_type, void* data);
int callback_dependency(int callback_type, void* data);
int callback_downtime(int callback_type, void* data);
int callback_event_handler(int callback_type, void* data);
int callback_external_command(int callback_type, void* data);
int callback_flapping_status(int callback_type, void* data);
int callback_group(int callback_type, void* data);
int callback_group_member(int callback_type, void* data);
int callback_host(int callback_type, void* data);
int callback_host_check(int callback_type, void* data);
int callback_host_status(int callback_type, void* data);
int callback_log(int callback_type, void* data);
int callback_module(int callback_type, void* data);
int callback_process(int callback_type, void* data);
int callback_program_status(int callback_type, void* data);
int callback_relation(int callback_type, void* data);
int callback_service(int callback_type, void* data);
int callback_service_check(int callback_type, void* data);
int callback_service_status(int callback_type, void* data);
void unregister_callbacks();
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_CALLBACKS_HH
