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

#include "com/centreon/broker/rrd/output.hh"

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/storage/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

namespace com {
namespace centreon {
namespace broker {
namespace rrd {
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
template <>
output<lib>::output(std::string const& metrics_path,
                    std::string const& status_path,
                    uint32_t cache_size,
                    bool ignore_update_errors,
                    bool write_metrics,
                    bool write_status)
    : io::stream("RRD"), _ignore_update_errors(ignore_update_errors),
      _metrics_path(metrics_path),
      _status_path(status_path),
      _write_metrics(write_metrics),
      _write_status(write_status),
      _backend(!metrics_path.empty() ? metrics_path : status_path, cache_size) {
}

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
template <>
output<cached<asio::local::stream_protocol::socket>>::output(
    std::string const& metrics_path,
    std::string const& status_path,
    uint32_t cache_size,
    bool ignore_update_errors,
    std::string const& local,
    bool write_metrics,
    bool write_status)
    : io::stream("RRD"), _ignore_update_errors(ignore_update_errors),
      _metrics_path(metrics_path),
      _status_path(status_path),
      _write_metrics(write_metrics),
      _write_status(write_status),
      _backend(metrics_path, cache_size) {
  _backend.connect_local(local);
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
template <>
output<cached<asio::ip::tcp::socket>>::output(std::string const& metrics_path,
                                              std::string const& status_path,
                                              uint32_t cache_size,
                                              bool ignore_update_errors,
                                              unsigned short port,
                                              bool write_metrics,
                                              bool write_status)
    : io::stream("RRD"), _ignore_update_errors(ignore_update_errors),
      _metrics_path(metrics_path),
      _status_path(status_path),
      _write_metrics(write_metrics),
      _write_status(write_status),
      _backend(metrics_path, cache_size) {
  _backend.connect_remote("localhost", port);
}
}  // namespace rrd
}  // namespace broker
}  // namespace centreon
}  // namespace com

/**
 *  Read data.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method throws.
 */
template <typename T>
bool output<T>::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw com::centreon::broker::exceptions::shutdown()
      << "cannot read from RRD stream";
  return true;
}

/**
 *  Update backend after a sigup.
 */
template <typename T>
void output<T>::update() {
  _backend.clean();
}

/**
 *  Write an event.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged.
 */
template <typename T>
int output<T>::write(std::shared_ptr<io::data> const& d) {
  log_v2::perfdata()->debug("RRD: output::write.");
  // Check that data exists.
  if (!validate(d, "RRD"))
    return 1;

  switch (d->type()) {
    case storage::metric::static_type():
      if (_write_metrics) {
        // Debug message.
        std::shared_ptr<storage::metric> e(
            std::static_pointer_cast<storage::metric>(d));
        log_v2::perfdata()->debug("RRD: new data for metric {} (time {}) {}",
                                  e->metric_id, e->ctime,
                                  e->is_for_rebuild ? "for rebuild" : "");

        // Metric path.
        std::string metric_path(
            fmt::format("{}{}.rrd", _metrics_path, e->metric_id));

        // Check that metric is not being rebuild.
        rebuild_cache::iterator it(_metrics_rebuild.find(metric_path));
        if (e->is_for_rebuild || it == _metrics_rebuild.end()) {
          // Write metrics RRD.
          try {
            _backend.open(metric_path);
          } catch (exceptions::open const& b) {
            time_t interval(e->interval ? e->interval : 60);
            assert(e->rrd_len);
            _backend.open(metric_path, e->rrd_len, e->ctime - 1, interval,
                          e->value_type);
          }
          std::ostringstream oss;
          switch (e->value_type) {
            case storage::perfdata::gauge:
              oss << std::fixed << e->value;
              log_v2::perfdata()->trace(
                  "RRD: update metric {} of type GAUGE with {}", e->metric_id,
                  oss.str());
              break;
            case storage::perfdata::counter:
              oss << static_cast<uint64_t>(e->value);
              log_v2::perfdata()->trace(
                  "RRD: update metric {} of type COUNTER with {}", e->metric_id,
                  oss.str());
              break;
            case storage::perfdata::derive:
              oss << static_cast<int64_t>(e->value);
              log_v2::perfdata()->trace(
                  "RRD: update metric {} of type DERIVE with {}", e->metric_id,
                  oss.str());
              break;
            case storage::perfdata::absolute:
              oss << static_cast<uint64_t>(e->value);
              log_v2::perfdata()->trace(
                  "RRD: update metric {} of type ABSOLUTE with {}",
                  e->metric_id, oss.str());
              break;
            default:
              oss << std::fixed << e->value;
              log_v2::perfdata()->trace(
                  "RRD: update metric {} of type {} with {}", e->metric_id,
                  e->value_type, oss.str());
              break;
          }
          _backend.update(e->ctime, oss.str());
        } else
          // Cache value.
          it->second.push_back(d);
      }
      break;
    case storage::status::static_type():
      if (_write_status) {
        // Debug message.
        std::shared_ptr<storage::status> e(
            std::static_pointer_cast<storage::status>(d));
        log_v2::perfdata()->debug(
            "RRD: new status data for index {} (state {}) {}", e->index_id,
            e->state, e->is_for_rebuild ? "for rebuild" : "");

        // Status path.
        std::string status_path(
            fmt::format("{}{}.rrd", _status_path, e->index_id));

        // Check that status is not begin rebuild.
        rebuild_cache::iterator it(_status_rebuild.find(status_path));
        if (e->is_for_rebuild || it == _status_rebuild.end()) {
          // Write status RRD.
          try {
            _backend.open(status_path);
          } catch (exceptions::open const& b) {
            time_t interval(e->interval ? e->interval : 60);
            assert(e->rrd_len);
            _backend.open(status_path, e->rrd_len, e->ctime - 1, interval);
          }
          std::string value;
          if (e->state == 0)
            value = "100";
          else if (e->state == 1)
            value = "75";
          else if (e->state == 2)
            value = "0";
          else
            value = "";
          _backend.update(e->ctime, value);
        } else
          // Cache value.
          it->second.push_back(d);
      }
      break;
    case storage::rebuild::static_type(): {
      // Debug message.
      std::shared_ptr<storage::rebuild> e(
          std::static_pointer_cast<storage::rebuild>(d));
      log_v2::perfdata()->debug("RRD: rebuild request for {} {} {}",
                                e->is_index ? "index" : "metric", e->id,
                                e->end ? "(end)" : "(start)");

      // Generate path.
      std::string path(fmt::format(
          "{}{}.rrd", e->is_index ? _status_path : _metrics_path, e->id));

      // Rebuild is starting.
      if (!e->end) {
        if (e->is_index)
          _status_rebuild[path];
        else
          _metrics_rebuild[path];
        _backend.remove(path);
      }
      // Rebuild is ending.
      else {
        // Find cache.
        std::list<std::shared_ptr<io::data>> l;
        {
          rebuild_cache::iterator it;
          if (e->is_index) {
            it = _status_rebuild.find(path);
            if (it != _status_rebuild.end()) {
              l = it->second;
              _status_rebuild.erase(it);
            }
          } else {
            it = _metrics_rebuild.find(path);
            if (it != _metrics_rebuild.end()) {
              l = it->second;
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
    } break;
    case storage::remove_graph::static_type(): {
      // Debug message.
      std::shared_ptr<storage::remove_graph> e(
          std::static_pointer_cast<storage::remove_graph>(d));
      log_v2::perfdata()->debug("RRD: remove graph request for {} {}",
                                e->is_index ? "index" : "metric", e->id);

      // Generate path.
      std::string path(fmt::format(
          "{}{}.rrd", e->is_index ? _status_path : _metrics_path, e->id));

      // Remove data from cache.
      rebuild_cache& cache(e->is_index ? _status_rebuild : _metrics_rebuild);
      rebuild_cache::iterator it(cache.find(path));
      if (it != cache.end())
        cache.erase(it);

      // Remove file.
      _backend.remove(path);
    } break;
  }

  return 1;
}
