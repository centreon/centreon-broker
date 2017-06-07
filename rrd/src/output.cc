/*
** Copyright 2011-2015,2017 Centreon
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

#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/cached.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"
#include "com/centreon/broker/rrd/output.hh"
#include "com/centreon/broker/storage/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Standard constructor.
 *
 *  @param[in] metrics_path         Path in which metrics RRD files
 *                                  should be written.
 *  @param[in] status_path          Path in which status RRD files
 *                                  should be written.
 *  @param[in] cache_size           The maximum number of cache element.
 *  @param[in] ignore_update_errors Set to true to ignore update errors.
 *  @param[in] write_metrics        Set to true if metrics graph must be
 *                                  written.
 *  @param[in] write_status         Set to true if status graph must be
 *                                  written.
 */
output::output(
          QString const& metrics_path,
          QString const& status_path,
          unsigned int cache_size,
          bool ignore_update_errors,
          bool write_metrics,
          bool write_status)
  : _backend(new lib(
                   (!metrics_path.isEmpty()
                    ? metrics_path.toStdString()
                    : status_path.toStdString()),
                   cache_size)),
    _ignore_update_errors(ignore_update_errors),
    _metrics_path(metrics_path.toStdString()),
    _status_path(status_path.toStdString()),
    _write_metrics(write_metrics),
    _write_status(write_status) {}

/**
 *  Local socket constructor.
 *
 *  @param[in] metrics_path         See standard constructor.
 *  @param[in] status_path          See standard constructor.
 *  @param[in] cache_size           The maximum number of cache element.
 *  @param[in] ignore_update_errors Set to true to ignore update errors.
 *  @param[in] local                Local socket connection parameters.
 *  @param[in] write_metrics        Set to true if metrics graph must be
 *                                  written.
 *  @param[in] write_status         Set to true if status graph must be
 *                                  written.
 */
output::output(
          QString const& metrics_path,
          QString const& status_path,
          unsigned int cache_size,
          bool ignore_update_errors,
          QString const& local,
          bool write_metrics,
          bool write_status)
  : _ignore_update_errors(ignore_update_errors),
    _metrics_path(metrics_path.toStdString()),
    _status_path(status_path.toStdString()),
    _write_metrics(write_metrics),
    _write_status(write_status) {
#if QT_VERSION >= 0x040400
  std::auto_ptr<cached>
    rrdcached(new cached(metrics_path.toStdString(), cache_size));
  rrdcached->connect_local(local);
  _backend.reset(rrdcached.release());
#else
  throw (broker::exceptions::msg()
         << "RRD: local connection is not supported on Qt "
         << QT_VERSION_STR);
#endif // Qt version
}

/**
 *  Network socket constructor.
 *
 *  @param[in] metrics_path         See standard constructor.
 *  @param[in] status_path          See standard constructor.
 *  @param[in] cache_size           The maximum number of cache element.
 *  @param[in] ignore_update_errors Set to true to ignore update errors.
 *  @param[in] port                 rrdcached listening port.
 *  @param[in] write_metrics        Set to true if metrics graph must be
 *                                  written.
 *  @param[in] write_status         Set to true if status graph must be
 *                                  written.
 */
output::output(
          QString const& metrics_path,
          QString const& status_path,
          unsigned int cache_size,
          bool ignore_update_errors,
          unsigned short port,
          bool write_metrics,
          bool write_status)
  : _ignore_update_errors(ignore_update_errors),
    _metrics_path(metrics_path.toStdString()),
    _status_path(status_path.toStdString()),
    _write_metrics(write_metrics),
    _write_status(write_status) {
  std::auto_ptr<cached>
    rrdcached(new cached(metrics_path.toStdString(), cache_size));
  rrdcached->connect_remote("localhost", port);
  _backend.reset(rrdcached.release());
}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Read data.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method throws.
 */
bool output::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (com::centreon::broker::exceptions::shutdown()
         << "cannot read from RRD stream");
  return (true);
}

/**
 *  Update backend after a sigup.
 */
void output::update() {
  if (_backend.get())
    _backend->clean();
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged.
 */
int output::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (!validate(d, "RRD"))
    return (1);

  if (d->type() == storage::metric::static_type()) {
    if (_write_metrics) {
      // Debug message.
      misc::shared_ptr<storage::metric>
        e(d.staticCast<storage::metric>());
      logging::debug(logging::medium) << "RRD: new data for metric "
        << e->metric_id << " (time " << e->ctime << ") "
        << (e->is_for_rebuild ? " for rebuild" : "");

      // Metric path.
      std::string metric_path;
      {
        std::ostringstream oss;
        oss << _metrics_path << e->metric_id << ".rrd";
        metric_path = oss.str();
      }

      // Check that metric is not being rebuild.
      rebuild_cache::iterator
        it(_metrics_rebuild.find(metric_path.c_str()));
      if (e->is_for_rebuild || it == _metrics_rebuild.end()) {
        // Write metrics RRD.
        try {
          _backend->open(metric_path);
        }
        catch (exceptions::open const& b) {
          time_t interval(e->interval ? e->interval : 60);
          unsigned int length(e->rrd_len / interval);
          _backend->open(
            metric_path,
            length,
            e->ctime - 1,
            interval,
            e->value_type);
        }
        std::ostringstream oss;
        if (e->value_type == storage::perfdata::counter)
          oss << static_cast<unsigned long long>(e->value);
        else if (e->value_type == storage::perfdata::derive)
          oss << static_cast<long long>(e->value);
        else
          oss << std::fixed << e->value;
      
        unsigned int modulo;
        modulo = e->ctime % e->interval;
        if (modulo > 0) {
            e->ctime = e->ctime - modulo;
        }
        _backend->update(e->ctime, oss.str());
      }
      else
        // Cache value.
        it->push_back(d);
    }
  }
  else if (d->type() == storage::status::static_type()) {
    if (_write_status) {
      // Debug message.
      misc::shared_ptr<storage::status>
        e(d.staticCast<storage::status>());
      logging::debug(logging::medium)
        << "RRD: new status data for index " << e->index_id << " ("
        << e->state << ") " << (e->is_for_rebuild ? "for rebuild" : "");

      // Status path.
      std::string status_path;
      {
        std::ostringstream oss;
        oss << _status_path << e->index_id << ".rrd";
        status_path = oss.str();
      }

      // Check that status is not begin rebuild.
      rebuild_cache::iterator
        it(_status_rebuild.find(status_path.c_str()));
      if (e->is_for_rebuild || it == _status_rebuild.end()) {
        // Write status RRD.
        try {
          _backend->open(status_path);
        }
        catch (exceptions::open const& b) {
          unsigned int
            length(e->rrd_len / (e->interval ? e->interval : 60));
          ++length;
          _backend->open(
            status_path,
            length,
            e->ctime - 1,
            e->interval);
        }
        std::ostringstream oss;
        if (e->state == 0)
          oss << 100;
        else if (e->state == 1)
          oss << 75;
        else if (e->state == 2)
          oss << 0;
      
        unsigned int modulo;
        modulo = e->ctime % e->interval;
        if (modulo > 0) {
            e->ctime = e->ctime - modulo;
        }
        _backend->update(e->ctime, oss.str());
      }
      else
        // Cache value.
        it->push_back(d);
    }
  }
  else if (d->type() == storage::rebuild::static_type()) {
    // Debug message.
    misc::shared_ptr<storage::rebuild>
      e(d.staticCast<storage::rebuild>());
    logging::debug(logging::medium) << "RRD: rebuild request for "
      << (e->is_index ? "index " : "metric ") << e->id
      << (e->end ? "(end)" : "(start)");

    // Generate path.
    std::string path;
    {
      std::ostringstream oss;
      oss << (e->is_index ? _status_path : _metrics_path)
          << e->id << ".rrd";
      path = oss.str();
    }

    // Rebuild is starting.
    if (!e->end) {
      if (e->is_index)
        _status_rebuild[path.c_str()];
      else
        _metrics_rebuild[path.c_str()];
      _backend->remove(path);
    }
    // Rebuild is ending.
    else {
      // Find cache.
      std::list<misc::shared_ptr<io::data> > l;
      {
        rebuild_cache::iterator it;
        if (e->is_index) {
          it = _status_rebuild.find(path.c_str());
          if (it != _status_rebuild.end()) {
            l = *it;
            _status_rebuild.erase(it);
          }
        }
        else {
          it = _metrics_rebuild.find(path.c_str());
          if (it != _metrics_rebuild.end()) {
            l = *it;
            _metrics_rebuild.erase(it);
          }
        }
      }

      // Resend cache data.
      while (!l.empty()) {
        write(l.front());
        l.pop_front();
      }
    }
  }
  else if (d->type() == storage::remove_graph::static_type()) {
    // Debug message.
    misc::shared_ptr<storage::remove_graph>
      e(d.staticCast<storage::remove_graph>());
    logging::debug(logging::medium) << "RRD: remove graph request for "
      << (e->is_index ? "index " : "metric ") << e->id;

    // Generate path.
    std::string path;
    {
      std::ostringstream oss;
      oss << (e->is_index ? _status_path : _metrics_path)
          << e->id << ".rrd";
      path = oss.str();
    }

    // Remove data from cache.
    rebuild_cache&
      cache(e->is_index ? _status_rebuild : _metrics_rebuild);
    rebuild_cache::iterator it(cache.find(path.c_str()));
    if (it != cache.end())
      cache.erase(it);

    // Remove file.
    _backend->remove(path);
  }

  return (1);
}
