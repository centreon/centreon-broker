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

#include "events/program_status.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the ProgramStatus object to the current
 *         instance.
 *
 *  Copy data defined within the ProgramStatus class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  \param[in] ps Object to copy data from.
 */
void ProgramStatus::InternalCopy(const ProgramStatus& ps)
{
  this->active_host_checks_enabled     = ps.active_host_checks_enabled;
  this->active_service_checks_enabled  = ps.active_service_checks_enabled;
  this->global_host_event_handler      = ps.global_host_event_handler;
  this->global_service_event_handler   = ps.global_service_event_handler;
  this->last_alive                     = ps.last_alive;
  this->last_command_check             = ps.last_command_check;
  this->last_log_rotation              = ps.last_log_rotation;
  this->modified_host_attributes       = ps.modified_host_attributes;
  this->obsess_over_hosts              = ps.obsess_over_hosts;
  this->obsess_over_services           = ps.obsess_over_services;
  this->passive_host_checks_enabled    = ps.passive_host_checks_enabled;
  this->passive_service_checks_enabled = ps.passive_service_checks_enabled;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief ProgramStatus default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
ProgramStatus::ProgramStatus()
  : active_host_checks_enabled(false),
    active_service_checks_enabled(false),
    last_alive(0),
    last_command_check(0),
    last_log_rotation(0),
    modified_host_attributes(0),
    modified_service_attributes(0),
    obsess_over_hosts(false),
    obsess_over_services(false),
    passive_host_checks_enabled(false),
    passive_service_checks_enabled(false) {}

/**
 *  \brief ProgramStatus copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  \param[in] ps Object to copy data from.
 */
ProgramStatus::ProgramStatus(const ProgramStatus& ps) : Status(ps)
{
  this->InternalCopy(ps);
}

/**
 *  ProgramStatus destructor.
 */
ProgramStatus::~ProgramStatus() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  \param[in] ps Object to copy data from.
 */
ProgramStatus& ProgramStatus::operator=(const ProgramStatus& ps)
{
  this->Status::operator=(ps);
  this->InternalCopy(ps);
  return (*this);
}

/**
 *  \brief Get the type of the event (Event::PROGRAMSTATUS).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  \return Event::PROGRAMSTATUS
 */
int ProgramStatus::GetType() const
{
  return (Event::PROGRAMSTATUS);
}
