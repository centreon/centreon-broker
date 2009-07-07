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
# include "events/program_status.h"
# include "events/service.h"
# include "events/service_status.h"

namespace                                       CentreonBroker
{
  // Acknowledgement
  extern DB::Mapping<Events::Acknowledgement>   acknowledgement_mapping;
  // Comment
  extern DB::Mapping<Events::Comment>           comment_mapping;
  // Connection
  extern DB::Mapping<Events::Connection>        connection_mapping;
  // ConnectionStatus
  extern DB::Mapping<Events::ConnectionStatus>  connection_status_mapping;
  // Downtime
  extern DB::Mapping<Events::Downtime>          downtime_mapping;
  // Host
  extern DB::Mapping<Events::Host>              host_mapping;
  // HostGroup
  extern DB::Mapping<Events::HostGroup>         host_group_mapping;
  // HostStatus
  extern DB::Mapping<Events::HostStatus>        host_status_mapping;
  // ProgramStatus
  extern DB::Mapping<Events::ProgramStatus>     program_status_mapping;
  // Service
  extern DB::Mapping<Events::Service>           service_mapping;
  // ServiceStatus
  extern DB::Mapping<Events::ServiceStatus>     service_status_mapping;

  void                                          InitMappings();
}

#endif /* !MAPPING_H_ */
