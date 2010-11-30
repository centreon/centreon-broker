/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CALLBACKS_H_
# define CALLBACKS_H_

int callback_acknowledgement(int callback_type, void* data);
int callback_comment(int callback_type, void* data);
int callback_downtime(int callback_type, void* data);
int callback_event_handler(int callback_type, void* data);
int callback_flapping_status(int callback_type, void* data);
int callback_host_check(int callback_type, void* data);
int callback_host_status(int callback_type, void* data);
int callback_log(int callback_type, void* data);
int callback_process(int callback_type, void* data);
int callback_program_status(int callback_type, void* data);
int callback_service_check(int callback_type, void* data);
int callback_service_status(int callback_type, void* data);

#endif /* !CALLBACKS_H_ */
