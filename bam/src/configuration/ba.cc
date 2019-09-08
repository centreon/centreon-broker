/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/bam/configuration/ba.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] id             BA ID.
 *  @param[in] name           BA name.
 *  @param[in] warning_level  BA warning_level.
 *  @param[in] critical_level BA critical_level.
 *  @param[in] inherit_kpi_downtime  Should the BA inherit kpi's downtimes?
 */
ba::ba(unsigned int id,
       std::string const& name,
       double warning_level,
       double critical_level,
       bool inherit_kpi_downtime)
    : _id(id),
      _host_id(0),
      _service_id(0),
      _name(name),
      _warning_level(warning_level),
      _critical_level(critical_level),
      _inherit_kpi_downtime(inherit_kpi_downtime) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other The original object.
 */
ba::ba(ba const& other)
    : _id(other._id),
      _host_id(other._host_id),
      _service_id(other._service_id),
      _name(other._name),
      _warning_level(other._warning_level),
      _critical_level(other._critical_level),
      _event(other._event),
      _inherit_kpi_downtime(other._inherit_kpi_downtime) {}

/**
 *  Destructor.
 */
ba::~ba() {}

/**
 *  Assignment Operator.
 *
 *  @param[in] other The original object.
 *
 *  @return this
 */
ba& ba::operator=(ba const& other) {
  if (this != &other) {
    _id = other._id;
    _host_id = other._host_id;
    _service_id = other._service_id;
    _name = other._name;
    _warning_level = other._warning_level;
    _critical_level = other._critical_level;
    _event = other._event;
    _inherit_kpi_downtime = other._inherit_kpi_downtime;
  }
  return (*this);
}

/**
 *  Equality comparison operator.
 *
 *  @param[in] right Object to compare to.
 *
 *  @return True if this object and right are totally equal.
 */
bool ba::operator==(ba const& right) const {
  return ((_id == right._id) && (_host_id == right._host_id) &&
          (_service_id == right._service_id) && (_name == right._name) &&
          (_warning_level == right._warning_level) &&
          (_critical_level == right._critical_level) &&
          (_event == right._event) &&
          (_inherit_kpi_downtime == right._inherit_kpi_downtime));
}

/**
 *  Inequality comparison operator.
 *
 *  @param[in] right Object to compare to.
 *
 *  @return True if this object and right are inequal.
 */
bool ba::operator!=(ba const& right) const {
  return (!operator==(right));
}

/**
 *  Get business activity id.
 *
 *  @return An integer representing the value of a business activity.
 */
unsigned int ba::get_id() const {
  return (_id);
}

/**
 *  Get the host ID.
 *
 *  @return BA host ID.
 */
unsigned int ba::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the id of the service associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
unsigned int ba::get_service_id() const {
  return (_service_id);
}

/**
 *  Get name of the business activity.
 *
 *  @return The name.
 */
std::string const& ba::get_name() const {
  return (_name);
}

/**
 *  Get warning level.
 *
 *  @return The percentage for the warning level.
 */
double ba::get_warning_level() const {
  return (_warning_level);
}

/**
 *  Get critical level.
 *
 *  @return The percentage for the critical level.
 */
double ba::get_critical_level() const {
  return (_critical_level);
}

/**
 *  Get the opened event of this ba.
 *
 *  @return  The opened event of this ba.
 */
com::centreon::broker::bam::ba_event const& ba::get_opened_event() const {
  return (_event);
}

/**
 *  Get if the BA should inherit the downtime of its kpis.
 *
 *  @return  True if the BA should inherit the downtime of its kpis.
 */
bool ba::get_inherit_kpi_downtime() const {
  return (_inherit_kpi_downtime);
}

/**
 *  Set id.
 *
 *  @param[in] id Set business activity id key.
 */
void ba::set_id(unsigned int id) {
  _id = id;
}

/**
 *  Set the service id associated to this ba.
 *
 *  @param[in] service_id  Set the service id.
 */
void ba::set_host_id(unsigned int host_id) {
  _host_id = host_id;
}

/**
 *  Set the service id associated to this ba.
 *
 *  @param[in] service_id  Set the service id.
 */
void ba::set_service_id(unsigned int service_id) {
  _service_id = service_id;
}

/**
 *  Set name.
 *
 *  @param[in] name Name of the BA.
 */
void ba::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set the percentage for warning level
 *
 *  @param[in] warning_level Warning level.
 */
void ba::set_warning_level(double warning_level) {
  _warning_level = warning_level;
}

/**
 *  Set the percentage for criticial level.
 *
 *  @param[in] critical_level Critical level.
 */
void ba::set_critical_level(double critical_level) {
  _critical_level = critical_level;
}

/**
 *  Set the current opened event for this BA.
 *
 *  @param[in] e  The current opened event.
 */
void ba::set_opened_event(bam::ba_event const& e) {
  _event = e;
}

/**
 *  Set the value of the inherit kpi downtime flag.
 *
 *  @param[in] value The value of the inherit kpi downtime flag.
 */
void ba::set_inherit_kpi_downtime(bool value) {
  _inherit_kpi_downtime = value;
}
