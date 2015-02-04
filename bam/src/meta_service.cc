/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cmath>
#include "com/centreon/broker/bam/meta_service.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
meta_service::meta_service()
  : _computation(meta_service::average),
    _id(0),
    _last_state(-1),
    _level_critical(0.0),
    _level_warning(0.0),
    _recompute_count(0),
    _value(NAN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
meta_service::meta_service(meta_service const& other)
  : computable(other), metric_listener(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
meta_service::~meta_service() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
meta_service& meta_service::operator=(meta_service const& other) {
  if (this != &other) {
    computable::operator=(other);
    metric_listener::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Add some metric that will impact this meta-service.
 *
 *  @param[in] metric_id  Metric ID.
 */
void meta_service::add_metric(unsigned int metric_id) {
  _metrics[metric_id] = 0.0;
  _recompute_count = _recompute_limit;
  return ;
}

/**
 *  Unused callback.
 *
 *  @param[in] child     Unused.
 *  @param[out] visitor  Unused.
 *
 *  @return              True.
 */
bool meta_service::child_has_update(
                     computable* child,
                     io::stream* visitor) {
  (void)child;
  (void)visitor;
  return true;
}

/**
 *  Get meta-service ID.
 *
 *  @return Meta-service ID.
 */
unsigned int meta_service::get_id() const {
  return (_id);
}

/**
 *  Get meta-service state.
 *
 *  @return Current meta-service state.
 */
short meta_service::get_state() const {
  short state;
  bool less_than(_level_warning < _level_critical);
  if ((less_than && (_value >= _level_critical))
      || (!less_than && (_value <= _level_critical)))
    state = 2;
  else if ((less_than && (_value >= _level_warning))
           || (!less_than && (_value <= _level_warning)))
    state = 1;
  else if (isnan(_value))
    state = 3;
  else
    state = 0;
  return (state);
}

/**
 *  Some child of the meta-service has a status update.
 *
 *  @param[in]  m        Metric update.
 *  @param[out] visitor  Visitor that will receive meta-service status.
 */
void meta_service::metric_update(
                     misc::shared_ptr<storage::metric> const& m,
                     io::stream* visitor) {
  if (!m.isNull()) {
    umap<unsigned int, double>::iterator
      it(_metrics.find(m->metric_id));
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
        visit(visitor);
      }
    }
  }
  return ;
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
      umap<unsigned int, double>::const_iterator
        it(_metrics.begin()),
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
      umap<unsigned int, double>::const_iterator
        it(_metrics.begin()),
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
    for (umap<unsigned int, double>::const_iterator
           it(_metrics.begin()),
           end(_metrics.end());
         it != end;
         ++it)
      _value += it->second;
    if (_computation != sum)
      _value /= _metrics.size();
  }
  _recompute_count = 0;
  return ;
}

/**
 *  Remove metric from meta-service.
 *
 *  @param[in] metric_id  Metric ID.
 */
void meta_service::remove_metric(unsigned int metric_id) {
  _metrics.erase(metric_id);
  _recompute_count = _recompute_limit;
  return ;
}

/**
 *  Set computation method.
 *
 *  @param[in] type  Computation method.
 */
void meta_service::set_computation(
                     meta_service::computation_type type) {
  _computation = type;
  _recompute_count = _recompute_limit;
  return ;
}

/**
 *  Set meta-service ID.
 *
 *  @param[in] id  Meta-service ID.
 */
void meta_service::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set critical level.
 *
 *  @param[in] level  Critical level.
 */
void meta_service::set_level_critical(double level) {
  _level_critical = level;
  return ;
}

/**
 *  Set warning level.
 *
 *  @param[in] level  Warning level.
 */
void meta_service::set_level_warning(double level) {
  _level_warning = level;
  return ;
}

/**
 *  Visit meta-service.
 *
 *  @param[out] visitor  Visitor that will receive meta-service status.
 */
void meta_service::visit(io::stream* visitor) {
  if (visitor) {
    // Recompute out-of-date value.
    if (_recompute_count >= _recompute_limit)
      recompute();

    // Send meta-service status.
    misc::shared_ptr<meta_service_status>
      status(new meta_service_status);
    short new_state(get_state());
    status->meta_service_id = _id;
    status->value = _value;
    status->state_changed = (_last_state != new_state);
    _last_state = new_state;
    logging::debug(logging::low)
      << "BAM: generating status of meta-service "
      << status->meta_service_id << " (value " << status->value
      << ")";
    visitor->write(status.staticCast<io::data>());
  }
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void meta_service::_internal_copy(meta_service const& other) {
  _computation = other._computation;
  _id = other._id;
  _last_state = other._last_state;
  _level_critical = other._level_critical;
  _level_warning = other._level_warning;
  _metrics = other._metrics;
  _recompute_count = _recompute_limit;
  _value = other._value;
  return ;
}

/**
 *  Perform a partial recomputation of the vlaue.
 *
 *  @param[in] new_value  New value.
 *  @param[in] old_value  Old value.
 */
void meta_service::_recompute_partial(
                     double new_value,
                     double old_value) {
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
  return ;
}
