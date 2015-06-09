/*
** Copyright 2011-2012,2015 Merethis
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

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QLinkedList>
#include "com/centreon/broker/config/applier/endpoint.hh"
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
static config::applier::endpoint* gl_endpoint = NULL;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Comparison classes.
 */
class                  failover_match_name {
public:
                       failover_match_name(QString const& fo)
    : _failover(fo) {}
                       failover_match_name(failover_match_name const& fmn)
    : _failover(fmn._failover) {}
                       ~failover_match_name() {}
  failover_match_name& operator=(failover_match_name& fmn) {
    _failover = fmn._failover;
    return (*this);
  }
  bool                 operator()(config::endpoint const& endp) const {
    return (_failover == endp.name);
  }

private:
  QString              _failover;
};
class                  name_match_failover {
public:
                       name_match_failover(QString const& name)
    : _name(name) {}
                       name_match_failover(name_match_failover const& nmf)
    : _name(nmf._name) {}
                       ~name_match_failover() {}
  name_match_failover& operator=(name_match_failover const& nmf) {
    _name = nmf._name;
    return (*this);
  }
  bool                 operator()(config::endpoint const& endp) const {
    return (endp.failover == _name
              || endp.secondary_failovers.find(_name)
                   != endp.secondary_failovers.end());
  }

private:
  QString              _name;
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
 *  @param[in] endpoints        Endpoints configuration objects.
 *  @param[in] cache_directory  Endpoint cache directory.
 */
void endpoint::apply(
                 std::list<config::endpoint> const& endpoints,
                 std::string const& cache_directory) {
  // Log messages.
  logging::config(logging::medium)
    << "endpoint applier: loading configuration";
  logging::debug(logging::high) << "endpoint applier: "
    << endpoints.size() << " endpoints to apply";

  // Copy endpoint configurations and apply eventual modifications.
  _cache_directory = cache_directory;
  std::list<config::endpoint> tmp_endpoints(endpoints);
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it1(io::protocols::instance().begin()),
         end1(io::protocols::instance().end());
       it1 != end1;
       ++it1) {
    for (std::list<config::endpoint>::iterator
           it2(tmp_endpoints.begin()),
           end2(tmp_endpoints.end());
         it2 != end2;
         ++it2)
      it1->endpntfactry->has_endpoint(*it2);
  }

  // Remove old inputs and generate inputs to create.
  std::list<config::endpoint> endp_to_create;
  {
    QMutexLocker lock(&_endpointsm);
    _diff_endpoints(
      _endpoints,
      tmp_endpoints,
      endp_to_create);
  }

  // Update existing endpoints.
  for (iterator it(_endpoints.begin()), end(_endpoints.end());
       it != end;
       ++it)
    it->second->update();

  // Debug message.
  logging::debug(logging::high) << "endpoint applier: "
    << endp_to_create.size() << " endpoints to create";

  // Create new endpoints.
  for (std::list<config::endpoint>::iterator
         it(endp_to_create.begin()),
         end(endp_to_create.end());
       it != end;
       ++it) {
    // Check that output is not a failover.
    if (it->name.isEmpty()
        || (std::find_if(endp_to_create.begin(),
              endp_to_create.end(),
              name_match_failover(it->name))
            == endp_to_create.end())) {
      // Create subscriber and endpoint.
      misc::shared_ptr<multiplexing::subscriber>
        s(_create_subscriber(*it));
      bool is_acceptor;
      misc::shared_ptr<io::endpoint>
        e(_create_endpoint(*it, is_acceptor));
      std::auto_ptr<processing::thread> endp;
      if (is_acceptor) {
        std::auto_ptr<processing::acceptor>
          acceptr(new processing::acceptor(
                                    e,
                                    it->name.toStdString(),
                                    cache_directory));
        acceptr->set_read_filters(_filters(it->read_filters));
        acceptr->set_write_filters(_filters(it->write_filters));
        endp.reset(acceptr.release());
      }
      else
        endp.reset(_create_failover(*it, s, e, endp_to_create));
      {
        QMutexLocker lock(&_endpointsm);
        _endpoints[*it] = endp.get();
      }

      // Run thread.
      logging::debug(logging::medium) << "endpoint applier: endpoint "
           "thread " << endp.get() << " of '" << it->name
        << "' is registered and ready to run";
      endp.release()->start();
    }
  }

  return ;
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
    QMutexLocker lock(&_endpointsm);

    // Send termination requests.
    for (iterator it(_endpoints.begin()), end(_endpoints.end());
         it != end;
         ++it)
      it->second->exit();

    // Wait for threads.
    while (!_endpoints.empty()) {
      // Print remaining thread count.
      logging::debug(logging::low) << "endpoint applier: "
        << _endpoints.size() << " endpoint threads remaining";
      lock.unlock();
      time_t now(time(NULL));
      do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      } while (time(NULL) <= now); // Maximum one second delay.

      // Expect threads to terminate.
      lock.relock();
      // With a map valid iterator are not invalidated by erase().
      for (iterator it(_endpoints.begin()), end(_endpoints.end());
           it != end;)
        if (it->second->wait(0)) {
          delete it->second;
          iterator to_delete(it);
          ++it;
          _endpoints.erase(to_delete);
        }
        else
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
  return (_endpoints.begin());
}

/**
 *  Get last iterator of endpoints.
 *
 *  @return Last iterator of endpoints.
 */
endpoint::iterator endpoint::endpoints_end() {
  return (_endpoints.end());
}

/**
 *  Get endpoints mutex.
 *
 *  @return Endpoints mutex.
 */
QMutex& endpoint::endpoints_mutex() {
  return (_endpointsm);
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
endpoint& endpoint::instance() {
  return (*gl_endpoint);
}

/**
 *  Load singleton.
 */
void endpoint::load() {
  if (!gl_endpoint)
    gl_endpoint = new endpoint;
  return ;
}

/**
 *  Unload singleton.
 */
void endpoint::unload() {
  delete gl_endpoint;
  gl_endpoint = NULL;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
endpoint::endpoint() : _endpointsm(QMutex::Recursive) {}

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
  uset<unsigned int> read_elements(_filters(cfg.read_filters));
  uset<unsigned int> write_elements(_filters(cfg.write_filters));

  // Create subscriber.
  std::auto_ptr<multiplexing::subscriber>
    s(new multiplexing::subscriber(
                          cfg.name.toStdString(),
                          _cache_directory,
                          true));
  s->get_muxer().set_read_filters(read_elements);
  s->get_muxer().set_write_filters(write_elements);
  return (s.release());
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
                                  misc::shared_ptr<multiplexing::subscriber> sbscrbr,
                                  misc::shared_ptr<io::endpoint> endp,
                                  std::list<config::endpoint>& l) {
  // Debug message.
  logging::config(logging::medium)
    << "endpoint applier: creating new endpoint '" << cfg.name << "'";

  // Check that failover is configured.
  misc::shared_ptr<processing::failover> failovr;
  if (!cfg.failover.isEmpty()) {
    std::list<config::endpoint>::iterator it(std::find_if(l.begin(), l.end(), failover_match_name(cfg.failover)));
    if (it == l.end())
      throw (exceptions::msg() << "endpoint applier: could not find " \
                  "failover '" << cfg.failover << "' for endpoint '"
               << cfg.name << "'");
    bool is_acceptor;
    misc::shared_ptr<io::endpoint>
      e(_create_endpoint(*it, is_acceptor));
    if (is_acceptor)
      throw (exceptions::msg()
             << "endpoint applier: cannot allow acceptor '"
             << cfg.failover << "' as failover for endpoint '"
             << cfg.name << "'");
    failovr = misc::shared_ptr<processing::failover>(
                _create_failover(
                  *it,
                  sbscrbr,
                  e,
                  l));
  }

  // Check secondary failovers
  std::vector<misc::shared_ptr<io::endpoint> > secondary_failovrs;
  for (std::set<QString>::const_iterator
         failover_it(cfg.secondary_failovers.begin()),
         failover_end(cfg.secondary_failovers.end());
       failover_it != failover_end;
       ++failover_it) {
    std::list<config::endpoint>::iterator it(std::find_if(l.begin(), l.end(), failover_match_name(*failover_it)));
    if (it == l.end())
      throw (exceptions::msg() << "endpoint applier: could not find " \
                  "secondary failover '" << *failover_it << "' for endpoint '"
               << cfg.name << "'");
    bool is_acceptor(false);
    secondary_failovrs.push_back(misc::shared_ptr<io::endpoint>(
                         _create_endpoint(
                           *it,
                           is_acceptor)));
    if (is_acceptor) {
      logging::error(logging::high)
        << "endpoint applier: secondary failover '"
        << *failover_it << "' is an acceptor and cannot therefore be "
        << "instantiated for endpoint '" << cfg.name << "'";
      secondary_failovrs.pop_back();
    }
  }

  // Return failover thread.
  std::auto_ptr<processing::failover>
    fo(new processing::failover(
                         endp,
                         sbscrbr,
                         cfg.name,
                         _cache_directory));
  fo->set_buffering_timeout(cfg.buffering_timeout);
  fo->set_read_timeout(cfg.read_timeout);
  fo->set_retry_interval(cfg.retry_interval);
  fo->set_failover(failovr);
  for (std::vector<misc::shared_ptr<io::endpoint> >::iterator
         it(secondary_failovrs.begin()),
         end(secondary_failovrs.end());
       it != end;
       ++it)
    failovr->add_secondary_endpoint(*it);
  return (fo.release());
}

/**
 *  Create a new endpoint object.
 *
 *  @param[in]  cfg          The config.
 *  @param[out] is_acceptor  Set to true if endpoint is an acceptor.
 *
 *  @return A new endpoint.
 */
misc::shared_ptr<io::endpoint> endpoint::_create_endpoint(
                                           config::endpoint& cfg,
                                           bool& is_acceptor) {
  // Create endpoint object.
  misc::shared_ptr<io::endpoint> endp;
  int level(0);
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg)) {
      misc::shared_ptr<persistent_cache> cache;
      if (cfg.cache_enabled) {
        std::string cache_path(_cache_directory);
        cache_path.append("/");
        cache_path.append(cfg.name.toStdString());
        cache = misc::shared_ptr<persistent_cache>(
                        new persistent_cache(cache_path));
      }
      endp = misc::shared_ptr<io::endpoint>(
                     it.value().endpntfactry->new_endpoint(
                                                cfg,
                                                is_acceptor,
                                                cache));
      level = it.value().osi_to + 1;
      break ;
    }
  }
  if (endp.isNull())
    throw (exceptions::msg() << "endpoint applier: no matching " \
             "type found for endpoint '" << cfg.name << "'");

  // Create remaining objects.
  while (level <= 7) {
    // Browse protocol list.
    QMap<QString, io::protocols::protocol>::const_iterator it(io::protocols::instance().begin());
    QMap<QString, io::protocols::protocol>::const_iterator end(io::protocols::instance().end());
    while (it != end) {
      if ((it.value().osi_from == level)
          && (it.value().endpntfactry->has_endpoint(cfg))) {
        misc::shared_ptr<io::endpoint>
          current(it.value().endpntfactry->new_endpoint(
                                             cfg,
                                             is_acceptor));
        current->from(endp);
        endp = current;
        level = it.value().osi_to;
        break ;
      }
      ++it;
    }
    if ((7 == level) && (it == end))
      throw (exceptions::msg() << "endpoint applier: no matching " \
               "protocol found for endpoint '" << cfg.name << "'");
    ++level;
  }

  return (endp);
}

/**
 *  Diff the current configuration with the new configuration.
 *
 *  @param[in]  current       Current endpoints.
 *  @param[in]  new_endpoints New endpoints configuration.
 *  @param[out] to_create     Endpoints that should be created.
 */
void endpoint::_diff_endpoints(
                 std::map<config::endpoint, processing::thread*> const& current,
                 std::list<config::endpoint> const& new_endpoints,
                 std::list<config::endpoint>& to_create) {
  // Copy some lists that we will modify.
  std::list<config::endpoint> new_ep(new_endpoints);
  std::map<config::endpoint, processing::thread*> to_delete(current);

  // Loop through new configuration.
  while (!new_ep.empty()) {
    // Find a root entry.
    std::list<config::endpoint>::iterator list_it(new_ep.begin());
    while ((list_it != new_ep.end())
           && !list_it->name.isEmpty()
           && (std::find_if(
                      new_ep.begin(),
                      new_ep.end(),
                      name_match_failover(list_it->name))
               != new_ep.end()))
      ++list_it;
    if (list_it == new_ep.end())
      throw (exceptions::msg() << "endpoint applier: error while " \
                                  "diff'ing new and old configuration");
    QLinkedList<config::endpoint> entries;
    entries.push_back(*list_it);
    new_ep.erase(list_it);

    // Find all subentries.
    for (QLinkedList<config::endpoint>::iterator
           it_entries(entries.begin()),
           it_end(entries.end());
         it_entries != it_end;
         ++it_entries) {
      // Find primary failover.
        if (!it_entries->failover.isEmpty()) {
          list_it = std::find_if(
                           new_ep.begin(),
                           new_ep.end(),
                           failover_match_name(it_entries->failover));
          if (list_it == new_ep.end())
            throw (exceptions::msg() << "endpoint applier: could not find "\
                   "failover '" << it_entries->failover
                   << "' for endpoint '" << it_entries->name << "'");
          entries.push_back(*list_it);
          new_ep.erase(list_it);
        }
        // Find secondary failovers.
        for (std::set<QString>::const_iterator
               failover_it(entries.last().secondary_failovers.begin()),
               failover_end(entries.last().secondary_failovers.end());
             failover_it != failover_end;
             ++failover_it) {
          list_it = std::find_if(
                           new_ep.begin(),
                           new_ep.end(),
                           failover_match_name(*failover_it));
          if (list_it == new_ep.end())
            throw (exceptions::msg() << "endpoint applier: could not find "\
                   "secondary failover '" << *failover_it
                   << "' for endpoint '" << it_entries->name << "'");
          entries.push_back(*list_it);
          new_ep.erase(list_it);
      }
    }

    // Try to find entry and subentries in the endpoints already running.
    iterator map_it(to_delete.find(entries.first()));
    if (map_it == to_delete.end())
      for (QLinkedList<config::endpoint>::iterator
             it(entries.begin()),
             end(entries.end());
           it != end;
           ++it)
        to_create.push_back(*it);
    else
      to_delete.erase(map_it);
  }

  // Remove old endpoints.
  for (iterator it(to_delete.begin()), end(to_delete.end());
       it != end;
       ++it) {
    // Send only termination request, object will be destroyed by event
    // loop on termination. But wait for threads because they hold
    // resources that might be used by other endpoints.
    it->second->exit();
    it->second->wait();
    delete it->second;
  }

  return ;
}

/**
 *  Create filters from a set of category.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return Filters.
 */
uset<unsigned int> endpoint::_filters(std::set<std::string> const& str_filters) {
  uset<unsigned int> elements;
  for (std::set<std::string>::const_iterator
         it(str_filters.begin()), end(str_filters.end());
       it != end;
       ++it) {
    io::events::events_container const&
      tmp_elements(io::events::instance().get_matching_events(*it));
    for (io::events::events_container::const_iterator
           it(tmp_elements.begin()),
           end(tmp_elements.end());
         it != end;
         ++it) {
      logging::config(logging::medium)
        << "endpoint applier: new filtering element: " << it->first;
      elements.insert(it->first);
    }
  }
  return (elements);
}
