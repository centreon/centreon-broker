/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NEB_CALLBACKS_HH
#  define CCB_NEB_CALLBACKS_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
  extern int
          gl_mod_flags;
  extern void*
          gl_mod_handle;

  int     callback_acknowledgement(int callback_type, void* data);
  int     callback_comment(int callback_type, void* data);
  int     callback_custom_variable(int callback_type, void* data);
  int     callback_downtime(int callback_type, void* data);
  int     callback_event_handler(int callback_type, void* data);
  int     callback_external_command(int callback_type, void* data);
  int     callback_flapping_status(int callback_type, void* data);
  int     callback_group(int callback_type, void* data);
  int     callback_group_member(int callback_type, void* data);
  int     callback_host(int callback_type, void* data);
  int     callback_host_check(int callback_type, void* data);
  int     callback_host_status(int callback_type, void* data);
  int     callback_log(int callback_type, void* data);
  int     callback_module(int callback_type, void* data);
  int     callback_process(int callback_type, void* data);
  int     callback_program_status(int callback_type, void* data);
  int     callback_relation(int callback_type, void* data);
  int     callback_service(int callback_type, void* data);
  int     callback_service_check(int callback_type, void* data);
  int     callback_service_status(int callback_type, void* data);
  void    unregister_callbacks();
}

CCB_END()

#endif /* !CCB_NEB_CALLBACKS_HH */
