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

#include <cstring>
#include <string>
#include "events/host_service.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the HostService object to the current instance.
 */
void HostService::InternalCopy(const HostService& hs)
{
  memcpy(this->bools_, hs.bools_, sizeof(this->bools_));
  memcpy(this->doubles_, hs.doubles_, sizeof(this->doubles_));
  memcpy(this->shorts_, hs.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = hs.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostService default constructor.
 */
HostService::HostService() throw ()
{
  memset(this->bools_, 0, sizeof(this->bools_));
  for (unsigned int i = 0; i < DOUBLE_NB; i++)
    this->doubles_[i] = 0.0;
  memset(this->shorts_, 0, sizeof(this->shorts_));
}

/**
 *  HostService copy constructor.
 */
HostService::HostService(const HostService& hs)
{
  this->InternalCopy(hs);
}

/**
 *  HostService destructor.
 */
HostService::~HostService() throw ()
{
}

/**
 *  HostService operator= overload.
 */
HostService& HostService::operator=(const HostService& hs)
{
  this->InternalCopy(hs);
  return (*this);
}

/**
 *  Get the action_url member.
 */
const std::string& HostService::GetActionUrl() const throw ()
{
  return (this->strings_[ACTION_URL]);
}

/**
 *  Get the check_freshness member.
 */
bool HostService::GetCheckFreshness() const throw ()
{
  return (this->bools_[CHECK_FRESHNESS]);
}

/**
 *  Get the display_name member.
 */
const std::string& HostService::GetDisplayName() const throw ()
{
  return (this->strings_[DISPLAY_NAME]);
}

/**
 *  Get the first_notification_delay member.
 */
double HostService::GetFirstNotificationDelay() const throw ()
{
  return (this->doubles_[FIRST_NOTIFICATION_DELAY]);
}

/**
 *  Get the freshness_threshold member.
 */
double HostService::GetFreshnessThreshold() const throw ()
{
  return (this->doubles_[FRESHNESS_THRESHOLD]);
}

/**
 *  Get the high_flap_threshold member.
 */
double HostService::GetHighFlapThreshold() const throw ()
{
  return (this->doubles_[HIGH_FLAP_THRESHOLD]);
}

/**
 *  Get the icon_image member.
 */
const std::string& HostService::GetIconImage() const throw ()
{
  return (this->strings_[ICON_IMAGE]);
}

/**
 *  Get the icon_image_alt member.
 */
const std::string& HostService::GetIconImageAlt() const throw ()
{
  return (this->strings_[ICON_IMAGE_ALT]);
}

/**
 *  Get the low_flap_threshold member.
 */
double HostService::GetLowFlapThreshold() const throw ()
{
  return (this->doubles_[LOW_FLAP_THRESHOLD]);
}

/**
 *  Get the notes member.
 */
const std::string& HostService::GetNotes() const throw ()
{
  return (this->strings_[NOTES]);
}

/**
 *  Get the notes_url member.
 */
const std::string& HostService::GetNotesUrl() const throw ()
{
  return (this->strings_[NOTES_URL]);
}

/**
 *  Get the notification_interval member.
 */
double HostService::GetNotificationInterval() const throw ()
{
  return (this->doubles_[NOTIFICATION_INTERVAL]);
}

/**
 *  Get the notify_on_downtime member.
 */
short HostService::GetNotifyOnDowntime() const throw ()
{
  return (this->shorts_[NOTIFY_ON_DOWNTIME]);
}

/**
 *  Get the notify_on_flapping member.
 */
short HostService::GetNotifyOnFlapping() const throw ()
{
  return (this->shorts_[NOTIFY_ON_FLAPPING]);
}

/**
 *  Get the notify_on_recovery member.
 */
short HostService::GetNotifyOnRecovery() const throw ()
{
  return (this->shorts_[NOTIFY_ON_RECOVERY]);
}

/**
 *  Get the retain_nonstatus_information member.
 */
bool HostService::GetRetainNonstatusInformation() const throw ()
{
  return (this->bools_[RETAIN_NONSTATUS_INFORMATION]);
}

/**
 *  Get the retain_status_information member.
 */
bool HostService::GetRetainStatusInformation() const throw ()
{
  return (this->bools_[RETAIN_STATUS_INFORMATION]);
}

/**
 *  Set the action_url member.
 */
void HostService::SetActionUrl(const std::string& au)
{
  this->strings_[ACTION_URL] = au;
  return ;
}

/**
 *  Set the check_freshness member.
 */
void HostService::SetCheckFreshness(bool cf) throw ()
{
  this->bools_[CHECK_FRESHNESS] = cf;
  return ;
}

/**
 *  Set the display_name member.
 */
void HostService::SetDisplayName(const std::string& dn)
{
  this->strings_[DISPLAY_NAME] = dn;
  return ;
}

/**
 *  Set the first_notification_delay member.
 */
void HostService::SetFirstNotificationDelay(double fnd) throw ()
{
  this->doubles_[FIRST_NOTIFICATION_DELAY] = fnd;
  return ;
}

/**
 *  Set the freshness_threshold member.
 */
void HostService::SetFreshnessThreshold(double ft) throw ()
{
  this->doubles_[FRESHNESS_THRESHOLD] = ft;
  return ;
}

/**
 *  Set the high_flap_threshold member.
 */
void HostService::SetHighFlapThreshold(double hft) throw ()
{
  this->doubles_[HIGH_FLAP_THRESHOLD] = hft;
  return ;
}

/**
 *  Set the icon_image member.
 */
void HostService::SetIconImage(const std::string& ii)
{
  this->strings_[ICON_IMAGE] = ii;
  return ;
}

/**
 *  Set the icon_image_alt member.
 */
void HostService::SetIconImageAlt(const std::string& iia)
{
  this->strings_[ICON_IMAGE_ALT] = iia;
  return ;
}

/**
 *  Set the low_flap_threshold member.
 */
void HostService::SetLowFlapThreshold(double lft) throw ()
{
  this->doubles_[LOW_FLAP_THRESHOLD] = lft;
  return ;
}

/**
 *  Set the notes member.
 */
void HostService::SetNotes(const std::string& n)
{
  this->strings_[NOTES] = n;
  return ;
}

/**
 *  Set the notes_url member.
 */
void HostService::SetNotesUrl(const std::string& nu)
{
  this->strings_[NOTES_URL] = nu;
  return ;
}

/**
 *  Set the notification_interval member.
 */
void HostService::SetNotificationInterval(double ni) throw ()
{
  this->doubles_[NOTIFICATION_INTERVAL] = ni;
  return ;
}

/**
 *  Set the notify_on_downtime member.
 */
void HostService::SetNotifyOnDowntime(short nod) throw ()
{
  this->shorts_[NOTIFY_ON_DOWNTIME] = nod;
  return ;
}

/**
 *  Set the notify_on_flapping member.
 */
void HostService::SetNotifyOnFlapping(short nof) throw ()
{
  this->shorts_[NOTIFY_ON_FLAPPING] = nof;
  return ;
}

/**
 *  Set the notify_on_recovery member.
 */
void HostService::SetNotifyOnRecovery(short nor) throw ()
{
  this->shorts_[NOTIFY_ON_RECOVERY] = nor;
  return ;
}

/**
 *  Set the retain_nonstatus_information member.
 */
void HostService::SetRetainNonstatusInformation(bool rni) throw ()
{
  this->bools_[RETAIN_NONSTATUS_INFORMATION] = rni;
  return ;
}

/**
 *  Set the retain_status_information member.
 */
void HostService::SetRetainStatusInformation(bool rsi) throw ()
{
  this->bools_[RETAIN_STATUS_INFORMATION] = rsi;
  return ;
}
