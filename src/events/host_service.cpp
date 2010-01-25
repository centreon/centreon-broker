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

#include "events/host_service.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy all internal data of the HostService object to the current
 *         instance.
 *
 *  Copy data defined within the HostService class. This is used by the copy
 *  constructor and the assignment operator.
 *
 *  \param[in] hs Object to copy data from.
 */
void HostService::InternalCopy(const HostService& hs)
{
  this->action_url                   = hs.action_url;
  this->check_freshness              = hs.check_freshness;
  this->display_name                 = hs.display_name;
  this->first_notification_delay     = hs.first_notification_delay;
  this->freshness_threshold          = hs.freshness_threshold;
  this->high_flap_threshold          = hs.high_flap_threshold;
  this->icon_image                   = hs.icon_image;
  this->icon_image_alt               = hs.icon_image_alt;
  this->low_flap_threshold           = hs.low_flap_threshold;
  this->notes                        = hs.notes;
  this->notes_url                    = hs.notes_url;
  this->notification_interval        = hs.notification_interval;
  this->notification_period          = hs.notification_period;
  this->notify_on_downtime           = hs.notify_on_downtime;
  this->notify_on_flapping           = hs.notify_on_flapping;
  this->notify_on_recovery           = hs.notify_on_recovery;
  this->retain_nonstatus_information = hs.retain_nonstatus_information;
  this->retain_status_information    = hs.retain_status_information;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief HostService default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
HostService::HostService()
  : check_freshness(false),
    first_notification_delay(0.0),
    freshness_threshold(0.0),
    high_flap_threshold(0.0),
    low_flap_threshold(0.0),
    notification_interval(0.0),
    notify_on_downtime(0),
    notify_on_flapping(0),
    notify_on_recovery(0),
    retain_nonstatus_information(false),
    retain_status_information(false) {}

/**
 *  \brief HostService copy constructor.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  \param[in] hs Object to copy data from.
 */
HostService::HostService(const HostService& hs)
{
  this->InternalCopy(hs);
}

/**
 *  HostService destructor.
 */
HostService::~HostService() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  \param[in] hs Object to copy data from.
 *
 *  \return *this
 */
HostService& HostService::operator=(const HostService& hs)
{
  this->InternalCopy(hs);
  return (*this);
}
