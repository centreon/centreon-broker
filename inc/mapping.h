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

# include "acknowledgement.h"
# include "comment.h"
# include "connection.h"
# include "connection_status.h"
# include "db/mapping.hpp"
# include "downtime.h"
# include "host.h"
# include "host_status.h"
# include "program_status.h"
# include "service.h"
# include "service_status.h"

namespace                               CentreonBroker
{
  // Acknowledgement
  extern DB::Mapping<Acknowledgement>   acknowledgement_mapping;
  // Comment
  extern DB::Mapping<Comment>           comment_mapping;
  // Connection
  extern DB::Mapping<Connection>        connection_mapping;
  // ConnectionStatus
  extern DB::Mapping<ConnectionStatus>  connection_status_mapping;
  // Downtime
  extern DB::Mapping<Downtime>          downtime_mapping;
  // Host
  extern DB::Mapping<Host>              host_mapping;
  // HostStatus
  extern DB::Mapping<HostStatus>        host_status_mapping;
  // ProgramStatus
  extern DB::Mapping<ProgramStatus>     program_status_mapping;
  // Service
  extern DB::Mapping<Service>           service_mapping;
  // ServiceStatus
  extern DB::Mapping<ServiceStatus>     service_status_mapping;

  void                                  InitMappings();
}

#endif /* !MAPPING_H_ */
