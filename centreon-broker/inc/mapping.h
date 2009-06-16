/*
** mapping.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/01/09 Matthieu Kermagoret
** Last update 06/16/09 Matthieu Kermagoret
*/

#ifndef MAPPING_H_
# define MAPPING_H_

# include "acknowledgement.h"
# include "connection.h"
# include "connection_status.h"
# include "db/mapping.hpp"
# include "host.h"
# include "host_status.h"
# include "program_status.h"
# include "service.h"
# include "service_status.h"

namespace                               CentreonBroker
{
  // Acknowledgement
  extern DB::Mapping<Acknowledgement>   acknowledgement_mapping;
  // Connection
  extern DB::Mapping<Connection>        connection_mapping;
  // ConnectionStatus
  extern DB::Mapping<ConnectionStatus>  connection_status_mapping;
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
