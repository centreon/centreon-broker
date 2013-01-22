/*
** Copyright 2011-2012 Merethis
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
#include <QCoreApplication>
#include <QMutexLocker>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/stringifier.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

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
    return (_name == endp.failover);
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
 *  @param[in] inputs    Inputs configuration.
 *  @param[in] outputs   Outputs configuration.
 *  @param[in] temporary Temporary configuration.
 */
void endpoint::apply(
                 QList<config::endpoint> const& inputs,
                 QList<config::endpoint> const& outputs,
                 config::endpoint temporary) {
  // Log messages.
  logging::config(logging::medium)
    << "endpoint applier: loading configuration";
  logging::debug(logging::high) << "endpoint applier: " << inputs.size()
    << " inputs and " << outputs.size() << " outputs to apply";

  bool temporary_has_change(false);
  std::auto_ptr<io::endpoint> endp_temporary;
  {
    QMutexLocker lock(&_temporarym);
    if (_temporary != temporary) {
      temporary_has_change = true;
      _temporary = temporary;
    }
    // Create temporary endpoint with temporary configuration.
    endp_temporary
      = std::auto_ptr<io::endpoint>(_create_temporary(temporary));
  }

  // Remove old inputs and generate inputs to create.
  QList<config::endpoint> in_to_create;
  {
    QMutexLocker lock(&_inputsm);
    _diff_endpoints(
      _inputs,
      inputs,
      temporary_has_change,
      in_to_create);
  }

  // Remove old outputs and generate outputs to create.
  QList<config::endpoint> out_to_create;
  {
    QMutexLocker lock(&_outputsm);
    _diff_endpoints(
      _outputs,
      outputs,
      temporary_has_change,
      out_to_create);
  }

  // Update existing endpoints.
  for (QMap<config::endpoint, processing::failover*>::iterator
         it(_outputs.begin()),
         end(_outputs.end());
       it != end;
       ++it)
    if (*it)
      (*it)->update();
  for (QMap<config::endpoint, processing::failover*>::iterator
         it(_inputs.begin()),
         end(_inputs.end());
       it != end;
       ++it)
    if (*it)
      (*it)->update();

  // Debug message.
  logging::debug(logging::high) << "endpoint applier: "
    << in_to_create.size() << " inputs to create, "
    << out_to_create.size() << " outputs to create";

  // Create new outputs.
  for (QList<config::endpoint>::iterator it = out_to_create.begin(),
         end = out_to_create.end();
       it != end;
       ++it)
    // Check that output is not a failover.
    if (it->name.isEmpty()
        || (std::find_if(out_to_create.begin(),
              out_to_create.end(),
              name_match_failover(it->name))
            == out_to_create.end())) {
      // Create endpoint.
      std::auto_ptr<processing::failover> endp(_create_endpoint(
                                                 *it,
                                                 false,
                                                 true,
                                                 endp_temporary.get(),
                                                 out_to_create));
      connect(endp.get(), SIGNAL(finished()), this, SLOT(terminated_output()));
      connect(endp.get(), SIGNAL(terminated()), this, SLOT(terminated_output()));
      connect(endp.get(), SIGNAL(finished()), endp.get(), SLOT(deleteLater()));
      {
        QMutexLocker lock(&_outputsm);
        _outputs[*it] = endp.get();
      }

      // Run thread.
      logging::debug(logging::medium) << "endpoint applier: output " \
        "thread " << endp.get() << " is registered and ready to run";
      endp.release()->start();
    }

  // Create new inputs.
  for (QList<config::endpoint>::iterator it = in_to_create.begin(),
         end = in_to_create.end();
       it != end;
       ++it)
    // Check that output is not a failover.
    if (it->name.isEmpty()
        || (std::find_if(in_to_create.begin(),
              in_to_create.end(),
              name_match_failover(it->name))
            == in_to_create.end())) {
      // Create endpoint.
      std::auto_ptr<processing::failover> endp(_create_endpoint(
                                                 *it,
                                                 true,
                                                 false,
                                                 endp_temporary.get(),
                                                 in_to_create));
      connect(endp.get(), SIGNAL(finished()), this, SLOT(terminated_input()));
      connect(endp.get(), SIGNAL(terminated()), this, SLOT(terminated_input()));
      connect(endp.get(), SIGNAL(finished()), endp.get(), SLOT(deleteLater()));
      {
        QMutexLocker lock(&_inputsm);
        _inputs[*it] = endp.get();
      }

      // Run thread.
      logging::debug(logging::medium)
        << "endpoint applier: input thread " << endp.get()
        << " is registered and ready to run";
      endp.release()->start();
    }

  return ;
}

/**
 *  Discard applied configuration.
 */
void endpoint::discard() {
  logging::debug(logging::high) << "endpoint applier: destruction";

  // Exit input threads.
  {
    logging::debug(logging::medium) << "endpoint applier: " \
      "requesting input threads termination";
    QMutexLocker lock(&_inputsm);

    // Send termination requests.
    for (QMap<config::endpoint, processing::failover*>::iterator
           it = _inputs.begin(),
           end = _inputs.end();
         it != end;
         ++it)
      (*it)->process(false, false);

    // Wait for threads.
    while (!_inputs.empty()) {
      logging::debug(logging::low) << "endpoint applier: "
        << _inputs.size() << " input threads remaining";
      lock.unlock();
      time_t now(time(NULL));
      do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      } while (time(NULL) <= now);
      lock.relock();
    }
    logging::debug(logging::medium) << "endpoint applier: all " \
      "input threads are terminated";
    _inputs.clear();
  }

  // Stop multiplexing.
  multiplexing::engine::instance().stop();

  // Exit output threads.
  {
    logging::debug(logging::medium) << "endpoint applier: " \
      "requesting output threads termination";
    QMutexLocker lock(&_outputsm);

    // Send termination requests.
    for (QMap<config::endpoint, processing::failover*>::iterator
           it = _outputs.begin(),
           end = _outputs.end();
         it != end;
         ++it)
      (*it)->process(false, false);

    // Wait for threads.
    while (!_outputs.empty()) {
      misc::stringifier thread_list;
      thread_list << *_outputs.begin();
      for (QMap<config::endpoint, processing::failover*>::iterator
             it = ++_outputs.begin(),
             end = _outputs.end();
           it != end;
           ++it)
        thread_list << ", " << *it;
      logging::debug(logging::low) << "endpoint applier: "
        << _outputs.size() << " output threads remaining: "
        << thread_list.data();
      lock.unlock();
      time_t now(time(NULL));
      do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      } while (time(NULL) <= now);
      lock.relock();
    }
    logging::debug(logging::medium) << "endpoint applier: all output " \
      "threads are terminated";
    _outputs.clear();
  }
}

/**
 *  Get iterator to the beginning of input endpoints.
 *
 *  @return Iterator to the first input endpoint.
 */
endpoint::iterator endpoint::input_begin() {
  return (_inputs.begin());
}

/**
 *  Get last iterator of input endpoints.
 *
 *  @return Last iterator of input endpoints.
 */
endpoint::iterator endpoint::input_end() {
  return (_inputs.end());
}

/**
 *  Get input endpoints mutex.
 *
 *  @return Input endpoints mutex.
 */
QMutex& endpoint::input_mutex() {
  return (_inputsm);
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
 *  Get iterator to the beginning of output endpoints.
 *
 *  @return Iterator to the first output endpoint.
 */
endpoint::iterator endpoint::output_begin() {
  return (_outputs.begin());
}

/**
 *  Get last iterator of output endpoints.
 *
 *  @return Last iterator of output endpoints.
 */
endpoint::iterator endpoint::output_end() {
  return (_outputs.end());
}

/**
 *  Get output endpoints mutex.
 *
 *  @return Output endpoints mutex.
 */
QMutex& endpoint::output_mutex() {
  return (_outputsm);
}

/**
 *  An input thread finished executing.
 */
void endpoint::terminated_input() {
  QObject* sendr(QObject::sender());
  logging::debug(logging::medium) << "endpoint applier: input thread "
    << sendr << " terminated";
  QMutexLocker lock(&_inputsm);
  QList<config::endpoint> keys(_inputs.keys(
    static_cast<processing::failover*>(sendr)));
  for (QList<config::endpoint>::iterator
         it = keys.begin(),
         end = keys.end();
       it != end;
       ++it)
    _inputs.remove(*it);
  return ;
}

/**
 *  An output thread finished executing.
 */
void endpoint::terminated_output() {
  QObject* sendr(QObject::sender());
  logging::debug(logging::medium) << "endpoint applier: output thread "
    << sendr << " terminated";
  QMutexLocker lock(&_outputsm);
  QList<config::endpoint> keys(_outputs.keys(
    static_cast<processing::failover*>(sendr)));
  for (QList<config::endpoint>::iterator
         it = keys.begin(),
         end = keys.end();
       it != end;
       ++it)
    _outputs.remove(*it);
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
endpoint::endpoint() : QObject(), _outputsm(QMutex::Recursive) {}

/**
 *  Create and register an endpoint according to configuration.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint will act as input.
 *  @param[in] is_output true if the endpoint will act as output.
 *  @param[in] temporary Temporary endpoint.
 *  @param[in] l         List of endpoints.
 */
processing::failover* endpoint::_create_endpoint(
                                  config::endpoint& cfg,
                                  bool is_input,
                                  bool is_output,
                                  io::endpoint const* temporary,
                                  QList<config::endpoint>& l) {
  // Debug message.
  logging::config(logging::medium)
    << "endpoint applier: creating new endpoint '" << cfg.name << "'";

  // Check that failover is configured.
  misc::shared_ptr<processing::failover> failovr;
  if (!cfg.failover.isEmpty()) {
    QList<config::endpoint>::iterator it(std::find_if(l.begin(), l.end(), failover_match_name(cfg.failover)));
    if (it == l.end())
      throw (exceptions::msg() << "endpoint applier: could not find " \
                  "failover '" << cfg.failover << "' for endpoint '"
               << cfg.name << "'");
    failovr = misc::shared_ptr<processing::failover>(
                      _create_endpoint(
                        *it,
                        is_input || is_output,
                        is_output,
                        temporary,
                        l));
  }

  // Create endpoint object.
  misc::shared_ptr<io::endpoint> endp;
  bool is_acceptor(false);
  int level(0);
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg, !is_output, is_output)) {
      endp = misc::shared_ptr<io::endpoint>(
                     it.value().endpntfactry->new_endpoint(
                                                cfg,
                                                is_input,
                                                is_output,
                                                temporary,
                                                is_acceptor));
      level = it.value().osi_to + 1;
      break ;
    }
  }
  if (endp.isNull())
    throw (exceptions::msg() << "endpoint applier: no matching " \
             "protocol found for endpoint '" << cfg.name << "'");

  // Create remaining objects.
  while (level <= 7) {
    // Browse protocol list.
    QMap<QString, io::protocols::protocol>::const_iterator it(io::protocols::instance().begin());
    QMap<QString, io::protocols::protocol>::const_iterator end(io::protocols::instance().end());
    while (it != end) {
      if ((it.value().osi_from == level)
          && (it.value().endpntfactry->has_endpoint(cfg, !is_output, is_output))) {
        misc::shared_ptr<io::endpoint>
          current(it.value().endpntfactry->new_endpoint(
                                             cfg,
                                             is_input,
                                             is_output,
                                             temporary,
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

  // Return failover thread.
  std::auto_ptr<processing::failover>
    fo(new processing::failover(is_output, temporary));
  fo->set_buffering_timeout(cfg.buffering_timeout);
  fo->set_name(cfg.name);
  fo->set_read_timeout(cfg.read_timeout);
  fo->set_retry_interval(cfg.retry_interval);
  fo->set_endpoint(endp);
  fo->set_failover(failovr);
  return (fo.release());
}

/**
 *  Create and register an temporary according to configuration.
 *
 *  @param[in] cfg Endpoint configuration.
 */
io::endpoint* endpoint::_create_temporary(config::endpoint& cfg) {
  // No temporary is define into the configuration.
  if (cfg.params.empty())
    return (NULL);

  // Debug message.
  logging::config(logging::medium)
    << "endpoint applier: creating new temporary '" << cfg.name << "'";

  // Check that failover is configured.
  if (!cfg.failover.isEmpty())
    throw (exceptions::msg() << "endpoint applier: find failover into "
           "temporary '" << cfg.name << "'");

  // Create endpoint object.
  std::auto_ptr<io::endpoint> endp;
  bool is_acceptor(false);
  int level(0);
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg, true, true)) {
      endp = std::auto_ptr<io::endpoint>(
                     it.value().endpntfactry->new_endpoint(
                                                cfg,
                                                false,
                                                false,
                                                NULL,
                                                is_acceptor));
      level = it.value().osi_to + 1;
      break ;
    }
  }
  if (!endp.get())
    throw (exceptions::msg() << "endpoint applier: no matching " \
             "protocol found for temporary '" << cfg.name << "'");

  // Create remaining objects.
  while (level <= 7) {
    // Browse protocol list.
    QMap<QString, io::protocols::protocol>::const_iterator it(io::protocols::instance().begin());
    QMap<QString, io::protocols::protocol>::const_iterator end(io::protocols::instance().end());
    while (it != end) {
      if ((it.value().osi_from == level)
          && (it.value().endpntfactry->has_endpoint(cfg, true, true))) {
        std::auto_ptr<io::endpoint>
          current(it.value().endpntfactry->new_endpoint(
                                             cfg,
                                             true,
                                             true,
                                             NULL,
                                             is_acceptor));
        current->from(misc::shared_ptr<io::endpoint>(endp.release()));
        endp = current;
        level = it.value().osi_to;
        break ;
      }
      ++it;
    }
    if ((7 == level) && (it == end))
      throw (exceptions::msg() << "endpoint applier: no matching " \
               "protocol found for temporary '" << cfg.name << "'");
    ++level;
  }
  return (endp.release());
}

/**
 *  Diff the current configuration with the new configuration.
 *
 *  @param[in]  current       Current endpoints.
 *  @param[in]  new_endpoints New endpoints configuration.
 *  @param[out] to_create     Endpoints that should be created.
 */
void endpoint::_diff_endpoints(
                 QMap<config::endpoint, processing::failover*> & current,
                 QList<config::endpoint> const& new_endpoints,
                 bool temporary_has_change,
                 QList<config::endpoint>& to_create) {
  // Copy some lists that we will modify.
  QList<config::endpoint> new_ep(new_endpoints);
  QMap<config::endpoint, processing::failover*> to_delete(current);

  // Loop through new configuration.
  while (!new_ep.isEmpty()) {
    // Find a root entry.
    QList<config::endpoint>::iterator list_it(new_ep.begin());
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
    QList<config::endpoint> entries;
    entries.push_back(*list_it);
    new_ep.erase(list_it);

    // Find all subentries.
    while (!entries.last().failover.isEmpty()) {
      list_it = std::find_if(
                       new_ep.begin(),
                       new_ep.end(),
                       failover_match_name(entries.last().failover));
      if (list_it == new_ep.end())
        throw (exceptions::msg() << "endpoint applier: could not find "\
               "failover '" << entries.last().failover
               << "' for endpoint '" << entries.last().name << "'");
      entries.push_back(*list_it);
      new_ep.erase(list_it);
    }

    // Try to find entry and subentries in the endpoints already running.
    QMap<config::endpoint, processing::failover*>::iterator
      map_it(to_delete.find(entries.first()));
    if (map_it == to_delete.end() || temporary_has_change)
      for (QList<config::endpoint>::iterator
             it(entries.begin()),
             end(entries.end());
           it != end;
           ++it)
        to_create.append(*it);
    else
      to_delete.erase(map_it);
  }

  // Remove old endpoints.
  for (QMap<config::endpoint, processing::failover*>::iterator it = to_delete.begin(),
         end = to_delete.end();
       it != end;
       ++it) {
    // Send only termination request, object will be destroyed by event
    // loop on termination. But wait for threads because they hold
    // resources that might be used by other endpoints.
    (*it)->process(false, false);
    (*it)->wait();
  }

  return ;
}
