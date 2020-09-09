/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/meta_service.hh"

#include <cmath>
#include <com/centreon/broker/bam/kpi_meta.hh>
#include <ctime>
#include <sstream>

#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
meta_service::meta_service()
    : _computation(meta_service::average),
      _id(0),
      _host_id(0),
      _service_id(0),
      _last_state(meta_service::state::state_unknown),
      _level_critical(0.0),
      _level_warning(0.0),
      _recompute_count(0),
      _value(NAN) {}

/**
 *  Destructor.
 */
meta_service::~meta_service() {}

/**
 *  Add some metric that will impact this meta-service.
 *
 *  @param[in] metric_id  Metric ID.
 */
void meta_service::add_metric(uint32_t metric_id) {
  _metrics[metric_id] = 0.0;
  _recompute_count = _recompute_limit;
}

/**
 *  Unused callback.
 *
 *  @param[in] child     Unused.
 *  @param[out] visitor  Unused.
 *
 *  @return              True.
 */
bool meta_service::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return true;
}

/**
 *  Get meta-service ID.
 *
 *  @return Meta-service ID.
 */
uint32_t meta_service::get_id() const {
  return (_id);
}

/**
 *  Get the meta-service's virtual host ID.
 *
 *  @return Virtual host ID.
 */
uint32_t meta_service::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the meta-service's virtual service ID.
 *
 *  @return Virtual service ID.
 */
uint32_t meta_service::get_service_id() const {
  return (_service_id);
}

/**
 *  Get meta-service output.
 *
 *  @return Meta-service output.
 */
std::string meta_service::get_output() const {
  std::ostringstream oss;
  oss << "Meta-Service " << _id;
  return (oss.str());
}

/**
 *  Get meta-service performance data.
 *
 *  @return Meta-servier performance data.
 */
std::string meta_service::get_perfdata() const {
  std::ostringstream oss;
  oss << "g[rta]=" << _value << ";" << _level_warning << ";" << _level_critical;
  return (oss.str());
}

/**
 *  Get meta-service state.
 *
 *  @return Current meta-service state.
 */
kpi_meta::state meta_service::get_state() const {
  kpi_meta::state state;
  bool less_than(_level_warning < _level_critical);
  if ((less_than && (_value >= _level_critical)) ||
      (!less_than && (_value <= _level_critical)))
    state = kpi_meta::state::state_critical;
  else if ((less_than && (_value >= _level_warning)) ||
           (!less_than && (_value <= _level_warning)))
    state = kpi_meta::state::state_warning;
  else if (std::isnan(_value))
    state = kpi_meta::state::state_unknown;
  else
    state = kpi_meta::state::state_ok;
  return (state);
}

/**
 *  Some child of the meta-service has a status update.
 *
 *  @param[in]  m        Metric update.
 *  @param[out] visitor  Visitor that will receive meta-service status.
 */
void meta_service::metric_update(std::shared_ptr<storage::metric> const& m,
                                 io::stream* visitor) {
  if (m) {
    bool state_has_changed = false;
    std::unordered_map<uint32_t, double>::iterator it(
        _metrics.find(m->metric_id));
    if (it != _metrics.end()) {
      if (it->second != m->value) {
        // Backup old value.
        double old_value(it->second);
        it->second = m->value;

        // Recompute.
        if (++_recompute_count >= _recompute_limit)
          recompute();
        else
          _recompute_partial(it->second, old_value);

        // Generate status event.
        visit(visitor, state_has_changed);
      }

      _send_service_status(visitor, state_has_changed);
    }
  }
}

/**
 *  Perform a full recomputation of the value.
 */
void meta_service::recompute() {
  // MIN.
  if (min == _computation) {
    if (_metrics.empty())
      _value = NAN;
    else {
      std::unordered_map<uint32_t, double>::const_iterator it(_metrics.begin()),
          end(_metrics.end());
      _value = it->second;
      while (++it != end)
        if (it->second < _value)
          _value = it->second;
    }
  }
  // MAX.
  else if (max == _computation) {
    if (_metrics.empty())
      _value = NAN;
    else {
      std::unordered_map<uint32_t, double>::const_iterator it(_metrics.begin()),
          end(_metrics.end());
      _value = it->second;
      while (++it != end)
        if (it->second > _value)
          _value = it->second;
    }
  }
  // SUM/AVERAGE.
  else {
    _value = 0.0;
    for (std::unordered_map<uint32_t, double>::const_iterator
             it(_metrics.begin()),
         end(_metrics.end());
         it != end; ++it)
      _value += it->second;
    if (_computation != sum)
      _value /= _metrics.size();
  }
  _recompute_count = 0;
}

/**
 *  Remove metric from meta-service.
 *
 *  @param[in] metric_id  Metric ID.
 */
void meta_service::remove_metric(uint32_t metric_id) {
  _metrics.erase(metric_id);
  _recompute_count = _recompute_limit;
}

/**
 *  Set computation method.
 *
 *  @param[in] type  Computation method.
 */
void meta_service::set_computation(meta_service::computation_type type) {
  _computation = type;
  _recompute_count = _recompute_limit;
}

/**
 *  Set meta-service ID.
 *
 *  @param[in] id  Meta-service ID.
 */
void meta_service::set_id(uint32_t id) {
  _id = id;
}

/**
 *  Set the meta-service's virtual host ID.
 *
 *  @param[in] host_id  Virtual host ID.
 */
void meta_service::set_host_id(uint32_t host_id) {
  _host_id = host_id;
}

/**
 *  Set the meta-service's virtual service ID.
 *
 *  @param[in] service_id  Virtual service ID.
 */
void meta_service::set_service_id(uint32_t service_id) {
  _service_id = service_id;
}

/**
 *  Set critical level.
 *
 *  @param[in] level  Critical level.
 */
void meta_service::set_level_critical(double level) {
  _level_critical = level;
}

/**
 *  Set warning level.
 *
 *  @param[in] level  Warning level.
 */
void meta_service::set_level_warning(double level) {
  _level_warning = level;
}

/**
 *  Visit meta-service.
 *
 *  @param[out] visitor        Visitor that will receive meta-service status.
 *  @param[out] changed_state  Set to true if the state was changed.
 */
void meta_service::visit(io::stream* visitor, bool& changed_state) {
  if (visitor) {
    // Recompute out-of-date value.
    if (_recompute_count >= _recompute_limit)
      recompute();

    // New state.
    kpi_meta::state new_state(get_state());
    changed_state = (_last_state != new_state);

    // Send meta-service status.
    {
      std::shared_ptr<meta_service_status> status(new meta_service_status);
      status->meta_service_id = _id;
      status->value = _value;
      status->state_changed = changed_state;
      _last_state = new_state;
      logging::debug(logging::low)
          << "BAM: generating status of meta-service "
          << status->meta_service_id << " (value " << status->value << ")";
      visitor->write(std::static_pointer_cast<io::data>(status));
    }
  }
}

/**
 *  Perform a partial recomputation of the vlaue.
 *
 *  @param[in] new_value  New value.
 *  @param[in] old_value  Old value.
 */
void meta_service::_recompute_partial(double new_value, double old_value) {
  // MIN.
  if (min == _computation) {
    if (new_value <= _value)
      _value = new_value;
    else if (_value == old_value)
      recompute();
  }
  // MAX.
  else if (max == _computation) {
    if (new_value >= _value)
      _value = new_value;
    else if (_value == old_value)
      recompute();
  }
  // SUM.
  else if (sum == _computation) {
    _value = _value - old_value + new_value;
  }
  // AVERAGE.
  else {
    _value = _value + (new_value - old_value) / _metrics.size();
  }
}

/**
 *  Send service status occasionally.
 *
 *  @param[out] visitor           The visitor.
 *  @param[in] state_has_changed  True if the state has changed.
 */
void meta_service::_send_service_status(io::stream* visitor,
                                        bool state_has_changed) {
  if (!visitor)
    return;

  time_t now(::time(nullptr));

  // Once every minutes, of if the state just changed.
  if (state_has_changed || _last_service_status_sent.is_null() ||
      std::difftime(_last_service_status_sent.get_time_t(), now) >= 60) {
    short new_state = get_state();
    std::shared_ptr<neb::service_status> status(new neb::service_status);
    status->active_checks_enabled = false;
    status->check_interval = 0.0;
    status->check_type = 1;  // Passive.
    status->current_check_attempt = 1;
    status->current_state = new_state;
    status->enabled = true;
    status->event_handler_enabled = false;
    status->execution_time = 0.0;
    status->flap_detection_enabled = false;
    status->has_been_checked = true;
    status->host_id = _host_id;
    status->is_flapping = false;
    status->last_check = time(nullptr);
    status->last_hard_state = new_state;
    status->last_hard_state_change = status->last_check;
    status->last_state_change = status->last_check;
    // status->last_time_critical = XXX;
    // status->last_time_unknown = XXX;
    // status->last_time_warning = XXX;
    status->last_update = time(nullptr);
    status->latency = 0.0;
    status->max_check_attempts = 1;
    status->obsess_over = false;
    status->output = get_output();
    // status->percent_state_chagne = XXX;
    status->perf_data = get_perfdata();
    status->retry_interval = 0;
    status->service_id = _service_id;
    status->should_be_scheduled = false;
    status->state_type = 1;  // Hard.
    visitor->write(status);
    _last_service_status_sent = now;
  }
}
