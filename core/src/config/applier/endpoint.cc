/*
** Copyright 2011-2012,2015,2017 Centreon
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

#include "com/centreon/broker/config/applier/endpoint.hh"
#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <vector>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/stringifier.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/processing/acceptor.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

// Class instance.
static config::applier::endpoint* gl_endpoint = nullptr;

/**************************************
 *                                     *
 *            Local Objects            *
 *                                     *
 **************************************/

/**
 *  Comparison classes.
 */
class failover_match_name {
 public:
  failover_match_name(std::string const& fo) : _failover(fo) {}
  failover_match_name(failover_match_name const& fmn)
      : _failover(fmn._failover) {}
  ~failover_match_name() {}
  failover_match_name& operator=(failover_match_name& fmn) {
    _failover = fmn._failover;
    return *this;
  }
  bool operator()(config::endpoint const& endp) const {
    return _failover == endp.name;
  }

 private:
  std::string _failover;
};
class name_match_failover {
 public:
  name_match_failover(std::string const& name) : _name(name) {}
  name_match_failover(name_match_failover const& nmf) : _name(nmf._name) {}
  ~name_match_failover() {}
  name_match_failover& operator=(name_match_failover const& nmf) {
    _name = nmf._name;
    return *this;
  }
  bool operator()(config::endpoint const& endp) const {
    return (std::find(endp.failovers.begin(), endp.failovers.end(), _name) !=
            endp.failovers.end());
  }

 private:
  std::string _name;
};

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Destructor.
 */
endpoint::~endpoint() {
  discard();
}

/**
 *  Apply the endpoint configuration.
 *
 *  @param[in] endpoints  Endpoints configuration objects.
 */
void endpoint::apply(std::list<config::endpoint> const& endpoints) {
  // Log messages.
  logging::config(logging::medium) << "endpoint applier: loading configuration";
  logging::debug(logging::high)
      << "endpoint applier: " << endpoints.size() << " endpoints to apply";

  // Copy endpoint configurations and apply eventual modifications.
  std::list<config::endpoint> tmp_endpoints(endpoints);
  for (std::map<std::string, io::protocols::protocol>::const_iterator
           it1(io::protocols::instance().begin()),
       end1(io::protocols::instance().end());
       it1 != end1; ++it1) {
    for (std::list<config::endpoint>::iterator it2(tmp_endpoints.begin()),
         end2(tmp_endpoints.end());
         it2 != end2; ++it2)
      it1->second.endpntfactry->has_endpoint(*it2);
  }

  // Remove old inputs and generate inputs to create.
  std::list<config::endpoint> endp_to_create;
  {
    std::lock_guard<std::timed_mutex> lock(_endpointsm);
    _diff_endpoints(_endpoints, tmp_endpoints, endp_to_create);
  }

  // Update existing endpoints.
  for (iterator it(_endpoints.begin()), end(_endpoints.end()); it != end; ++it)
    it->second->update();

  // Debug message.
  logging::debug(logging::high) << "endpoint applier: " << endp_to_create.size()
                                << " endpoints to create";

  // Create new endpoints.
  for (std::list<config::endpoint>::iterator it(endp_to_create.begin()),
       end(endp_to_create.end());
       it != end; ++it) {
    // Check that output is not a failover.
    if (it->name.empty() ||
        (std::find_if(endp_to_create.begin(), endp_to_create.end(),
                      name_match_failover(it->name)) == endp_to_create.end())) {
      // Create subscriber and endpoint.
      std::shared_ptr<multiplexing::subscriber> s(_create_subscriber(*it));
      bool is_acceptor;
      std::shared_ptr<io::endpoint> e(_create_endpoint(*it, is_acceptor));
      std::unique_ptr<processing::bthread> endp;
      if (is_acceptor) {
        std::unique_ptr<processing::acceptor> acceptr(
            new processing::acceptor(e, it->name));
        acceptr->set_read_filters(_filters(it->read_filters));
        acceptr->set_write_filters(_filters(it->write_filters));
        endp.reset(acceptr.release());
      } else
        endp.reset(_create_failover(*it, s, e, endp_to_create));
      {
        std::lock_guard<std::timed_mutex> lock(_endpointsm);
        _endpoints[*it] = endp.get();
      }

      // Run thread.
      logging::debug(logging::medium) << "endpoint applier: endpoint "
                                         "thread "
                                      << endp.get() << " of '" << it->name
                                      << "' is registered and ready to run";
      endp.release()->start();
    }
  }

  return;
}

/**
 *  Discard applied configuration.
 */
void endpoint::discard() {
  logging::debug(logging::high) << "endpoint applier: destruction";

  // Stop multiplexing.
  multiplexing::engine::instance().stop();

  // Exit threads.
  {
    logging::debug(logging::medium)
        << "endpoint applier: requesting threads termination";
    std::unique_lock<std::timed_mutex> lock(_endpointsm);

    // Send termination requests.
    for (iterator it(_endpoints.begin()), end(_endpoints.end()); it != end;
         ++it)
      it->second->exit();

    // Wait for threads.
    while (!_endpoints.empty()) {
      // Print remaining thread count.
      logging::debug(logging::low) << "endpoint applier: " << _endpoints.size()
                                   << " endpoint threads remaining";
      lock.unlock();

      // FIXME DBR: no more events without qt
      // do {
      //  QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      //} while (time(NULL) <= now); // Maximum one second delay.

      // Expect threads to terminate.
      lock.lock();
      // With a map valid iterator are not invalidated by erase().
      for (iterator it(_endpoints.begin()), end(_endpoints.end()); it != end;)
        if (it->second->wait(0)) {
          delete it->second;
          iterator to_delete(it);
          ++it;
          _endpoints.erase(to_delete);
        } else
          ++it;
    }
    logging::debug(logging::medium)
        << "endpoint applier: all threads are terminated";
    _endpoints.clear();
  }
}

/**
 *  Get iterator to the beginning of endpoints.
 *
 *  @return Iterator to the first endpoint.
 */
endpoint::iterator endpoint::endpoints_begin() {
  return _endpoints.begin();
}

/**
 *  Get last iterator of endpoints.
 *
 *  @return Last iterator of endpoints.
 */
endpoint::iterator endpoint::endpoints_end() {
  return _endpoints.end();
}

/**
 *  Get endpoints mutex.
 *
 *  @return Endpoints mutex.
 */
std::timed_mutex& endpoint::endpoints_mutex() {
  return _endpointsm;
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
endpoint& endpoint::instance() {
  return *gl_endpoint;
}

/**
 *  Load singleton.
 */
void endpoint::load() {
  if (!gl_endpoint)
    gl_endpoint = new endpoint;
  return;
}

/**
 *  Unload singleton.
 */
void endpoint::unload() {
  delete gl_endpoint;
  gl_endpoint = nullptr;
  return;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
endpoint::endpoint() {}

/**
 *  Create a muxer for a chain of failovers / endpoints. This method
 *  will create the subscriber of the chain and set appropriate filters.
 *
 *  @param[in] cfg  Configuration of the root endpoint.
 *
 *  @return The subscriber of the chain.
 */
multiplexing::subscriber* endpoint::_create_subscriber(config::endpoint& cfg) {
  // Build filtering elements.
  std::unordered_set<uint32_t> read_elements(_filters(cfg.read_filters));
  std::unordered_set<uint32_t> write_elements(_filters(cfg.write_filters));

  // Create subscriber.
  std::unique_ptr<multiplexing::subscriber> s(
      new multiplexing::subscriber(cfg.name, true));
  s->get_muxer().set_read_filters(read_elements);
  s->get_muxer().set_write_filters(write_elements);
  return s.release();
}

/**
 *  Create and register an endpoint according to configuration.
 *
 *  @param[in]  cfg      Endpoint configuration.
 *  @param[in]  sbscrbr  Subscriber.
 *  @param[in]  endp     Endpoint.
 *  @param[in]  l        List of endpoints.
 */
processing::failover* endpoint::_create_failover(
    config::endpoint& cfg,
    std::shared_ptr<multiplexing::subscriber> sbscrbr,
    std::shared_ptr<io::endpoint> endp,
    std::list<config::endpoint>& l) {
  // Debug message.
  logging::config(logging::medium)
      << "endpoint applier: creating new endpoint '" << cfg.name << "'";

  // Check that failover is configured.
  std::shared_ptr<processing::failover> failovr;
  if (!cfg.failovers.empty()) {
    std::string front_failover(cfg.failovers.front());
    std::list<config::endpoint>::iterator it(
        std::find_if(l.begin(), l.end(), failover_match_name(front_failover)));
    if (it == l.end())
      throw(exceptions::msg()
            << "endpoint applier: could not find "
               "failover '"
            << front_failover << "' for endpoint '" << cfg.name << "'");
    bool is_acceptor;
    std::shared_ptr<io::endpoint> e(_create_endpoint(*it, is_acceptor));
    if (is_acceptor)
      throw(exceptions::msg()
            << "endpoint applier: cannot allow acceptor '" << front_failover
            << "' as failover for endpoint '" << cfg.name << "'");
    failovr = std::shared_ptr<processing::failover>(
        _create_failover(*it, sbscrbr, e, l));

    // Add secondary failovers
    for (std::list<std::string>::const_iterator
             failover_it(++cfg.failovers.begin()),
         failover_end(cfg.failovers.end());
         failover_it != failover_end; ++failover_it) {
      std::list<config::endpoint>::iterator it(
          std::find_if(l.begin(), l.end(), failover_match_name(*failover_it)));
      if (it == l.end())
        throw(exceptions::msg()
              << "endpoint applier: could not find "
                 "secondary failover '"
              << *failover_it << "' for endpoint '" << cfg.name << "'");
      bool is_acceptor(false);
      std::shared_ptr<io::endpoint> endp(_create_endpoint(*it, is_acceptor));
      if (is_acceptor) {
        logging::error(logging::high)
            << "endpoint applier: secondary failover '" << *failover_it
            << "' is an acceptor and cannot therefore be "
            << "instantiated for endpoint '" << cfg.name << "'";
      }
      failovr->add_secondary_endpoint(endp);
    }
  }

  // Return failover thread.
  std::unique_ptr<processing::failover> fo(
      new processing::failover(endp, sbscrbr, cfg.name));
  fo->set_buffering_timeout(cfg.buffering_timeout);
  fo->set_retry_interval(cfg.retry_interval);
  fo->set_failover(failovr);
  return fo.release();
}

/**
 *  Create a new endpoint object.
 *
 *  @param[in]  cfg          The config.
 *  @param[out] is_acceptor  Set to true if endpoint is an acceptor.
 *
 *  @return A new endpoint.
 */
std::shared_ptr<io::endpoint> endpoint::_create_endpoint(config::endpoint& cfg,
                                                         bool& is_acceptor) {
  // Create endpoint object.
  std::shared_ptr<io::endpoint> endp;
  int level{0};
  for (std::map<std::string, io::protocols::protocol>::const_iterator
           it(io::protocols::instance().begin()),
       end(io::protocols::instance().end());
       it != end; ++it) {
    if ((it->second.osi_from == 1) &&
        it->second.endpntfactry->has_endpoint(cfg)) {
      std::shared_ptr<persistent_cache> cache;
      if (cfg.cache_enabled) {
        std::string cache_path(config::applier::state::instance().cache_dir());
        cache_path.append(".cache.");
        cache_path.append(cfg.name);
        cache =
            std::shared_ptr<persistent_cache>(new persistent_cache(cache_path));
      }
      endp = std::shared_ptr<io::endpoint>(
          it->second.endpntfactry->new_endpoint(cfg, is_acceptor, cache));
      level = it->second.osi_to + 1;
      break;
    }
  }
  if (!endp)
    throw exceptions::msg() << "endpoint applier: no matching "
                               "type found for endpoint '"
                            << cfg.name << "'";

  // Create remaining objects.
  while (level <= 7) {
    // Browse protocol list.
    std::map<std::string, io::protocols::protocol>::const_iterator it(
        io::protocols::instance().begin());
    std::map<std::string, io::protocols::protocol>::const_iterator end(
        io::protocols::instance().end());
    while (it != end) {
      if ((it->second.osi_from == level) &&
          (it->second.endpntfactry->has_endpoint(cfg))) {
        std::shared_ptr<io::endpoint> current(
            it->second.endpntfactry->new_endpoint(cfg, is_acceptor));
        current->from(endp);
        endp = current;
        level = it->second.osi_to;
        break;
      }
      ++it;
    }
    if (7 == level && it == end)
      throw exceptions::msg() << "endpoint applier: no matching "
                                 "protocol found for endpoint '"
                              << cfg.name << "'";
    ++level;
  }

  return endp;
}

/**
 *  Diff the current configuration with the new configuration.
 *
 *  @param[in]  current       Current endpoints.
 *  @param[in]  new_endpoints New endpoints configuration.
 *  @param[out] to_create     Endpoints that should be created.
 */
void endpoint::_diff_endpoints(
    std::map<config::endpoint, processing::bthread*> const& current,
    std::list<config::endpoint> const& new_endpoints,
    std::list<config::endpoint>& to_create) {
  // Copy some lists that we will modify.
  std::list<config::endpoint> new_ep(new_endpoints);
  std::map<config::endpoint, processing::bthread*> to_delete(current);

  // Loop through new configuration.
  while (!new_ep.empty()) {
    // Find a root entry.
    std::list<config::endpoint>::iterator list_it(new_ep.begin());
    while ((list_it != new_ep.end()) && !list_it->name.empty() &&
           (std::find_if(new_ep.begin(), new_ep.end(),
                         name_match_failover(list_it->name)) != new_ep.end()))
      ++list_it;
    if (list_it == new_ep.end())
      throw(exceptions::msg() << "endpoint applier: error while "
                                 "diff'ing new and old configuration");
    std::list<config::endpoint> entries;
    entries.push_back(*list_it);
    new_ep.erase(list_it);

    // Find all subentries.
    for (std::list<config::endpoint>::iterator it_entries(entries.begin()),
         it_end(entries.end());
         it_entries != it_end; ++it_entries) {
      // Find failovers.
      if (!it_entries->failovers.empty())
        for (std::list<std::string>::const_iterator
                 failover_it(it_entries->failovers.begin()),
             failover_end(it_entries->failovers.end());
             failover_it != failover_end; ++failover_it) {
          list_it = std::find_if(new_ep.begin(), new_ep.end(),
                                 failover_match_name(*failover_it));
          if (list_it == new_ep.end())
            throw(exceptions::msg()
                  << "endpoint applier: could not find failover '"
                  << *failover_it << "' for endpoint '" << it_entries->name
                  << "'");
          entries.push_back(*list_it);
          new_ep.erase(list_it);
        }
    }

    // Try to find entry and subentries in the endpoints already running.
    std::map<config::endpoint, processing::bthread*>::iterator map_it(
        to_delete.find(entries.front()));
    if (map_it == to_delete.end())
      for (std::list<config::endpoint>::iterator it(entries.begin()),
           end(entries.end());
           it != end; ++it)
        to_create.push_back(*it);
    else
      to_delete.erase(map_it);
  }

  // Remove old endpoints.
  for (auto it(to_delete.begin()), end(to_delete.end()); it != end; ++it) {
    // Send only termination request, object will be destroyed by event
    // loop on termination. But wait for threads because they hold
    // resources that might be used by other endpoints.
    it->second->exit();
    it->second->wait();
    delete it->second;
  }
}

/**
 *  Create filters from a set of category.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return Filters.
 */
std::unordered_set<uint32_t> endpoint::_filters(
    std::set<std::string> const& str_filters) {
  std::unordered_set<uint32_t> elements;
  for (std::set<std::string>::const_iterator it(str_filters.begin()),
       end(str_filters.end());
       it != end; ++it) {
    io::events::events_container const& tmp_elements(
        io::events::instance().get_matching_events(*it));
    for (io::events::events_container::const_iterator it(tmp_elements.begin()),
         end(tmp_elements.end());
         it != end; ++it) {
      logging::config(logging::medium)
          << "endpoint applier: new filtering element: " << it->first;
      elements.insert(it->first);
    }
  }
  return elements;
}
