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

#include "com/centreon/broker/bam/configuration/kpi.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 */
kpi::kpi(uint32_t id,
         short state_type,
         uint32_t host_id,
         uint32_t service_id,
         uint32_t ba_id,
         uint32_t indicator_ba,
         uint32_t meta_id,
         uint32_t boolexp_id,
         short status,
         short last_level,
         bool downtimed,
         bool acknowledged,
         bool ignore_downtime,
         bool ignore_acknowledgement,
         double warning,
         double critical,
         double unknown)
    : _id(id),
      _state_type(state_type),
      _host_id(host_id),
      _service_id(service_id),
      _ba_id(ba_id),
      _indicator_ba_id(indicator_ba),
      _meta_id(meta_id),
      _boolexp_id(boolexp_id),
      _status(status),
      _last_level(last_level),
      _downtimed(downtimed),
      _acknowledged(acknowledged),
      _ignore_downtime(ignore_downtime),
      _ignore_acknowledgement(ignore_acknowledgement),
      _impact_warning(warning),
      _impact_critical(critical),
      _impact_unknown(unknown),
      _event(_id, _ba_id) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other The copy.
 */
kpi::kpi(kpi const& other)
    : _id(other._id),
      _state_type(other._state_type),
      _host_id(other._host_id),
      _service_id(other._service_id),
      _ba_id(other._ba_id),
      _indicator_ba_id(other._indicator_ba_id),
      _meta_id(other._meta_id),
      _boolexp_id(other._boolexp_id),
      _status(other._status),
      _last_level(other._last_level),
      _downtimed(other._downtimed),
      _acknowledged(other._acknowledged),
      _ignore_downtime(other._ignore_downtime),
      _ignore_acknowledgement(other._ignore_acknowledgement),
      _impact_warning(other._impact_warning),
      _impact_critical(other._impact_critical),
      _impact_unknown(other._impact_unknown),
      _event(other._event) {}

/**
 *  Destructor.
 */
kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other The copy.
 *
 *  @return This object.
 */
kpi& kpi::operator=(kpi const& other) {
  if (&other != this) {
    _id = other._id;
    _state_type = other._state_type;
    _host_id = other._host_id;
    _service_id = other._service_id;
    _ba_id = other._ba_id;
    _indicator_ba_id = other._indicator_ba_id;
    _meta_id = other._meta_id;
    _boolexp_id = other._boolexp_id;
    _status = other._status;
    _last_level = other._last_level;
    _downtimed = other._downtimed;
    _acknowledged = other._acknowledged;
    _ignore_downtime = other._ignore_downtime;
    _ignore_acknowledgement = other._ignore_acknowledgement;
    _impact_warning = other._impact_warning;
    _impact_critical = other._impact_critical;
    _impact_unknown = other._impact_unknown;
    _event = other._event;
  }
  return (*this);
}

/**
 *  Equality comparison operator.
 *
 *  @param[in] other Object to compare to.
 *
 *  @return True if both objects are equal.
 */
bool kpi::operator==(kpi const& other) const {
  return ((_id == other._id) && (_state_type == other._state_type) &&
          (_host_id == other._host_id) && (_service_id == other._service_id) &&
          (_ba_id == other._ba_id) &&
          (_indicator_ba_id == other._indicator_ba_id) &&
          (_meta_id == other._meta_id) && (_boolexp_id == other._boolexp_id) &&
          (_status == other._status) && (_last_level == other._last_level) &&
          (_downtimed == other._downtimed) &&
          (_acknowledged == other._acknowledged) &&
          (_ignore_downtime == other._ignore_downtime) &&
          (_ignore_acknowledgement == other._ignore_acknowledgement) &&
          (_impact_warning == other._impact_warning) &&
          (_impact_critical == other._impact_critical) &&
          (_impact_unknown == other._impact_unknown) &&
          (_event == other._event));
}

/**
 *  Inequality comparison operator.
 *
 *  @param[in] other Object to compare to.
 *
 *  @return True if both objects are inequal.
 */
bool kpi::operator!=(kpi const& other) const {
  return (!operator==(other));
}

/**
 *  Get the id.
 *
 *  @return  The id value.
 */
unsigned kpi::get_id() const {
  return (_id);
}

/**
 *  Set the id.
 *
 *  @param id The id to set to the kpi.
 */
void kpi::set_id(uint32_t id) {
  _id = id;
}

/**
 *  Get the state type.
 *
 *  @return  The state type.
 */
short kpi::get_state_type() const {
  return (_state_type);
}

/**
 *  Get the host id.
 *
 *  @return  The host id.
 */
uint32_t kpi::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the service id.
 *
 *  @return  The service id.
 */
uint32_t kpi::get_service_id() const {
  return (_service_id);
}

/**
 *  Is this kpi a service ?
 *
 *  @return Whether this is a service or not.
 */
bool kpi::is_service() const {
  return (_service_id != 0);
}

/**
 *  Is this kpi a business activity ?
 *
 *  @return Whether this is a business activity abstraction or not.
 */
bool kpi::is_ba() const {
  return (_indicator_ba_id != 0);
}

/**
 *  Check if this KPI is a meta-service.
 *
 *  @return True if this KPI is a meta-service.
 */
bool kpi::is_meta() const {
  return (_meta_id != 0);
}

/**
 *  Check if this KPI is a boolean expression.
 *
 *  @return True if this KPI is a boolean expression.
 */
bool kpi::is_boolexp() const {
  return (_boolexp_id != 0);
}

/**
 *  Get ba id.
 *
 *  @return The id of the business activity.
 */
uint32_t kpi::get_ba_id() const {
  return (_ba_id);
}

/**
 *  Get indicator BA ID.
 *
 *  @return The ID of the BA attached to this KPI.
 */
uint32_t kpi::get_indicator_ba_id() const {
  return (_indicator_ba_id);
}

/**
 *  Get the meta-service ID.
 *
 *  @return Meta-ID of this KPI.
 */
uint32_t kpi::get_meta_id() const {
  return (_meta_id);
}

/**
 *  Get the boolean expression ID.
 *
 *  @return Boolean expression ID of this KPI.
 */
uint32_t kpi::get_boolexp_id() const {
  return (_boolexp_id);
}

/**
 *  Get the status.
 *
 *  @return The status.
 */
short kpi::get_status() const {
  return (_status);
}

/**
 *  Get the last level of this kpi.
 *
 *  @return The last level of this kpi.
 */
short kpi::get_last_level() const {
  return (_last_level);
}

/**
 *  Is this kpi set to downtime mode ?
 *
 *  @return Has this business interest been downtimed?
 */
bool kpi::is_downtimed() const {
  return (_downtimed);
}

/**
 *  Is this kpi acknowledged ?
 *
 *  @return Whether it has been acknowledged.
 */
bool kpi::is_acknowledged() const {
  return (_acknowledged);
}

/**
 *  Is this kpi to ignore downtime ?
 *
 *  @return Whether or not the downtime is relevant.
 */
bool kpi::ignore_downtime() const {
  return (_ignore_downtime);
}

/**
 *  Is this kpi to ignore acknowledgements ?
 *
 *  @return Whether or not the acknowledgements are applicable.
 */
bool kpi::ignore_acknowledgement() const {
  return (_ignore_acknowledgement);
}

/**
 *  Get impact warning.
 *
 *  @return The get business impact of a warning at this level.
 */
double kpi::get_impact_warning() const {
  return (_impact_warning);
}

/**
 *  Get impact critical.
 *
 *  @return The get business impact of a criticality at this level.
 */
double kpi::get_impact_critical() const {
  return (_impact_critical);
}

/**
 *  Get impact unknown.
 *
 *  @return The get business impact of an unknown status at this level.
 */
double kpi::get_impact_unknown() const {
  return (_impact_unknown);
}

/**
 *  Get the opened event associated with this kpi.
 *
 *  @return  The opened event associated with this kpi.
 */
com::centreon::broker::bam::kpi_event const& kpi::get_opened_event() const {
  return (_event);
}

/**
 *  Set state type.
 *
 *  @param[in] s Set the state type.
 */
void kpi::set_state_type(short s) {
  _state_type = s;
}

/**
 *  Set host id.
 *
 *  @param[in] i Set the host id.
 */
void kpi::set_host_id(uint32_t i) {
  _host_id = i;
}

/**
 *  Set service id.
 *
 *  @param[in] i The service id
 */
void kpi::set_service_id(uint32_t i) {
  _service_id = i;
}

/**
 *  Set the business activity id.
 *
 *  @param[in] i The business activity id.
 */
void kpi::set_ba_id(uint32_t i) {
  _ba_id = i;
}

/**
 *  Set the BA ID that affects this KPI.
 *
 *  @param[in] ba_id BA ID.
 */
void kpi::set_indicator_ba_id(uint32_t ba_id) {
  _indicator_ba_id = ba_id;
  return;
}

/**
 *  Set the meta-service ID that affects this KPI.
 *
 *  @param[in] meta_id  Meta-service ID.
 */
void kpi::set_meta_id(uint32_t meta_id) {
  _meta_id = meta_id;
  return;
}

/**
 *  Set the boolean expression ID that affects this KPI.
 *
 *  @param[in] boolexp_id  Boolean expression ID.
 */
void kpi::set_boolexp_id(uint32_t boolexp_id) {
  _boolexp_id = boolexp_id;
  return;
}

/**
 *  Set status.
 *
 *  @param[in] s Status value.
 */
void kpi::set_status(short s) {
  _status = s;
}

/**
 *  Set last_level.
 *
 *  @param[in] s Set the last level of the kpi.
 */
void kpi::set_last_level(short s) {
  _last_level = s;
}

/**
 *  Set downtimed.
 *
 *  @param[in] b Set whether the kpi is downtimed or not.
 */
void kpi::set_downtimed(bool b) {
  _downtimed = b;
}

/**
 *  Set acknowledged.
 *
 *  @param[in] b Set the acknowledged state.
 */
void kpi::set_acknowledged(bool b) {
  _acknowledged = b;
}

/**
 *  Ignore downtime.
 *
 *  @param[in] b Set whether or not the downtime is to be ignored.
 */
void kpi::ignore_downtime(bool b) {
  _ignore_downtime = b;
}

/**
 *  Ignore acknowledgement.
 *
 *  @param[in] b Set whether or not the acknowledgement is to be ignored.
 */
void kpi::ignore_acknowledgement(bool b) {
  _ignore_acknowledgement = b;
}

/**
 *  Set impact warning.
 *
 *  @param[in] d Set the business impact of a warning status.
 */
void kpi::set_impact_warning(double d) {
  _impact_warning = d;
}

/**
 *  Set impact critical.
 *
 *  @param[in] d Set the business impact of a critical status.
 */
void kpi::set_impact_critical(double d) {
  _impact_critical = d;
}

/**
 *  Set impact unknown.
 *
 *  @param[in] d Set the business impact of an unknown status.
 */
void kpi::set_impact_unknown(double d) {
  _impact_unknown = d;
}

/**
 *  Set the opened event associated with this kpi.
 *
 *  @param[in] kpi_event  The event.
 */
void kpi::set_opened_event(bam::kpi_event const& kpi_event) {
  _event = kpi_event;
}
