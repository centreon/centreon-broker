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

#include "events/host_service_status.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the given object to the current instance.
 *
 *  Make a copy of all internal members of HostServiceStatus to the current
 *  instance.
 *
 *  \param[in] hss Object to copy data from.
 */
void HostServiceStatus::InternalCopy(const HostServiceStatus& hss)
{
  this->acknowledgement_type          = hss.acknowledgement_type;
  this->active_checks_enabled         = hss.active_checks_enabled;
  this->check_command                 = hss.check_command;
  this->check_interval                = hss.check_interval;
  this->check_period                  = hss.check_period;
  this->check_type                    = hss.check_type;
  this->current_check_attempt         = hss.current_check_attempt;
  this->current_notification_number   = hss.current_notification_number;
  this->current_state                 = hss.current_state;
  this->event_handler                 = hss.event_handler;
  this->execution_time                = hss.execution_time;
  this->has_been_checked              = hss.has_been_checked;
  this->host                          = hss.host;
  this->host_id                       = hss.host_id;
  this->is_flapping                   = hss.is_flapping;
  this->last_check                    = hss.last_check;
  this->last_hard_state               = hss.last_hard_state;
  this->last_hard_state_change        = hss.last_hard_state_change;
  this->last_notification             = hss.last_notification;
  this->last_state_change             = hss.last_state_change;
  this->last_update                   = hss.last_update;
  this->latency                       = hss.latency;
  this->long_output                   = hss.long_output;
  this->max_check_attempts            = hss.max_check_attempts;
  this->modified_attributes           = hss.modified_attributes;
  this->next_check                    = hss.next_check;
  this->next_notification             = hss.next_notification;
  this->no_more_notifications         = hss.no_more_notifications;
  this->obsess_over                   = hss.obsess_over;
  this->output                        = hss.output;
  this->passive_checks_enabled        = hss.passive_checks_enabled;
  this->percent_state_change          = hss.percent_state_change;
  this->perf_data                     = hss.perf_data;
  this->problem_has_been_acknowledged = hss.problem_has_been_acknowledged;
  this->retry_interval                = hss.retry_interval;
  this->scheduled_downtime_depth      = hss.scheduled_downtime_depth;
  this->should_be_scheduled           = hss.should_be_scheduled;
  this->state_type                    = hss.state_type;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief HostServiceStatus default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
HostServiceStatus::HostServiceStatus()
  : acknowledgement_type(0),
    active_checks_enabled(false),
    check_interval(0.0),
    check_type(0),
    current_check_attempt(0),
    current_notification_number(0),
    current_state(4), // Pending
    execution_time(0.0),
    has_been_checked(false),
    host_id(0),
    is_flapping(false),
    last_check(0),
    last_hard_state(0),
    last_hard_state_change(0),
    last_notification(0),
    last_state_change(0),
    last_update(0),
    latency(0.0),
    max_check_attempts(0),
    modified_attributes(0),
    next_check(0),
    next_notification(0),
    no_more_notifications(false),
    obsess_over(false),
    passive_checks_enabled(false),
    percent_state_change(0.0),
    problem_has_been_acknowledged(false),
    retry_interval(0.0),
    scheduled_downtime_depth(0),
    should_be_scheduled(false),
    state_type(0) {}

/**
 *  \brief HostServiceStatus copy constructor.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  \param[in] hss Object to copy data from.
 */
HostServiceStatus::HostServiceStatus(const HostServiceStatus& hss)
  : Status(hss)
{
  this->InternalCopy(hss);
}

/**
 *  HostServiceStatus destructor.
 */
HostServiceStatus::~HostServiceStatus() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  \param[in] hss Object to copy data from.
 *
 *  \return *this
 */
HostServiceStatus& HostServiceStatus::operator=(const
  HostServiceStatus& hss)
{
  this->Status::operator=(hss);
  this->InternalCopy(hss);
  return (*this);
}
