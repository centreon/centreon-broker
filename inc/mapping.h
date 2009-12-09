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

# include "db/data_member.hpp"
# include "db/mapping.hpp"
# include "events/acknowledgement.h"
# include "events/comment.h"
# include "events/downtime.h"
# include "events/group.h"
# include "events/host.h"
# include "events/host_status.h"
# include "events/log.h"
# include "events/program_status.h"
# include "events/service.h"
# include "events/service_status.h"

// Acknowledgement
extern const CentreonBroker::DB::DataMember<Events::Acknowledgement> acknowledgement_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Acknowledgement>   acknowledgement_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Acknowledgement>   acknowledgement_set_mapping;
// Comment
extern const CentreonBroker::DB::DataMember<Events::Comment>
comment_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Comment>
comment_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Comment>
comment_set_mapping;
// Downtime
extern const CentreonBroker::DB::DataMember<Events::Downtime>
downtime_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Downtime>
downtime_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Downtime>
downtime_set_mapping;
// Group
extern const CentreonBroker::DB::DataMember<Events::Group>
group_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Group>
group_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Group>
group_set_mapping;
// Host
extern const CentreonBroker::DB::DataMember<Events::Host>
host_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Host>
host_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Host>
host_set_mapping;
// HostStatus
extern const CentreonBroker::DB::DataMember<Events::HostStatus>
host_status_dm[];
extern CentreonBroker::DB::MappingGetters<Events::HostStatus>
host_status_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::HostStatus>
host_status_set_mapping;
// Log
extern const CentreonBroker::DB::DataMember<Events::Log>
log_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Log>
log_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Log>
log_set_mapping;
// ProgramStatus
extern const CentreonBroker::DB::DataMember<Events::ProgramStatus>
program_status_dm[];
extern CentreonBroker::DB::MappingGetters<Events::ProgramStatus>
program_status_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::ProgramStatus>
program_status_set_mapping;
// Service
extern const CentreonBroker::DB::DataMember<Events::Service>
service_dm[];
extern CentreonBroker::DB::MappingGetters<Events::Service>
service_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::Service>
service_set_mapping;
// ServiceStatus
extern const CentreonBroker::DB::DataMember<Events::ServiceStatus>
service_status_dm[];
extern CentreonBroker::DB::MappingGetters<Events::ServiceStatus>
service_status_get_mapping;
extern CentreonBroker::DB::MappingSetters<Events::ServiceStatus>
service_status_set_mapping;

void     MappingsDestroy();
void     MappingsInit();

#endif /* !MAPPING_H_ */
