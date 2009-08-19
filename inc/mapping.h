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

#ifndef MAPPING_H_
# define MAPPING_H_

# include "db/mapping.hpp"
# include "events/acknowledgement.h"
# include "events/comment.h"
# include "events/connection.h"
# include "events/connection_status.h"
# include "events/downtime.h"
# include "events/host.h"
# include "events/host_group.h"
# include "events/host_status.h"
# include "events/log.h"
# include "events/program_status.h"
# include "events/service.h"
# include "events/service_status.h"

namespace  CentreonBroker
{
  // Acknowledgement
  extern DB::MappingGetters<Events::Acknowledgement>
           acknowledgement_get_mapping;
  extern DB::MappingSetters<Events::Acknowledgement>
           acknowledgement_set_mapping;
  // Comment
  extern DB::MappingGetters<Events::Comment>
           comment_get_mapping;
  extern DB::MappingSetters<Events::Comment>
           comment_set_mapping;
  // Connection
  extern DB::MappingGetters<Events::Connection>
           connection_get_mapping;
  extern DB::MappingSetters<Events::Connection>
           connection_set_mapping;
  // ConnectionStatus
  extern DB::MappingGetters<Events::ConnectionStatus>
           connection_status_get_mapping;
  extern DB::MappingSetters<Events::ConnectionStatus>
           connection_status_set_mapping;
  // Downtime
  extern DB::MappingGetters<Events::Downtime>
           downtime_get_mapping;
  extern DB::MappingSetters<Events::Downtime>
           downtime_set_mapping;
  // Host
  extern DB::MappingGetters<Events::Host>
           host_get_mapping;
  extern DB::MappingSetters<Events::Host>
           host_set_mapping;
  // HostGroup
  extern DB::MappingGetters<Events::HostGroup>
           host_group_get_mapping;
  extern DB::MappingSetters<Events::HostGroup>
           host_group_set_mapping;
  // HostStatus
  extern DB::MappingGetters<Events::HostStatus>
           host_status_get_mapping;
  extern DB::MappingSetters<Events::HostStatus>
           host_status_set_mapping;
  // Log
  extern DB::MappingGetters<Events::Log>
           log_get_mapping;
  extern DB::MappingSetters<Events::Log>
           log_set_mapping;
  // ProgramStatus
  extern DB::MappingGetters<Events::ProgramStatus>
           program_status_get_mapping;
  extern DB::MappingSetters<Events::ProgramStatus>
           program_status_set_mapping;
  // Service
  extern DB::MappingGetters<Events::Service>
           service_get_mapping;
  extern DB::MappingSetters<Events::Service>
           service_set_mapping;
  // ServiceStatus
  extern DB::MappingGetters<Events::ServiceStatus>
           service_status_get_mapping;
  extern DB::MappingSetters<Events::ServiceStatus>
           service_status_set_mapping;

  void     InitMappings();
}

#endif /* !MAPPING_H_ */
