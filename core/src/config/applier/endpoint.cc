/*
** Copyright 2011 Merethis
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
#include <assert.h>
#include <QCoreApplication>
#include <QMutexLocker>
#include <stdlib.h>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Comparison classes.
 */
class                  failover_match_name {
 private:
  QString              _failover;

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
};
class                  name_match_failover {
 private:
  QString              _name;

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
};

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
endpoint::endpoint() : QObject() {}

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] e Object to copy.
 */
endpoint::endpoint(endpoint const& e) : QObject() {
  (void)e;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& e) {
  (void)e;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Create and register an endpoint according to configuration.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint will act as input.
 *  @param[in] is_output true if the endpoint will act as output.
 *  @param[in] l         List of endpoints.
 */
processing::failover* endpoint::_create_endpoint(config::endpoint const& cfg,
                                                 bool is_input,
                                                 bool is_output,
                                                 QList<config::endpoint> const& l) {
  // Debug message.
  logging::config << logging::MEDIUM
    << "endpoint applier: creating new endpoint '" << cfg.name << "'";

  // Check that failover is configured.
  QSharedPointer<processing::failover> failovr;
  if (!cfg.failover.isEmpty()) {
    QList<config::endpoint>::const_iterator it(std::find_if(l.begin(), l.end(), failover_match_name(cfg.failover)));
    if (it == l.end())
      throw (exceptions::msg() << "endpoint applier: could not find " \
                  "failover '" << cfg.failover << "' for endpoint '"
               << cfg.name << "'");
    failovr = QSharedPointer<processing::failover>(_create_endpoint(*it, is_input || is_output, is_output, l));
  }
  
  // Create endpoint object.
  QSharedPointer<io::endpoint> endp;
  bool is_acceptor(false);
  int level(0);
  for (QMap<QString, io::protocols::protocol>::const_iterator it = io::protocols::instance().begin(),
         end = io::protocols::instance().end();
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg, !is_output, is_output)) {
      endp = QSharedPointer<io::endpoint>(it.value().endpntfactry->new_endpoint(cfg, is_input, is_output, is_acceptor));
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
        QSharedPointer<io::endpoint> current(it.value().endpntfactry->new_endpoint(cfg, !is_output, is_output, is_acceptor));
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
  QScopedPointer<processing::failover> fo(new processing::failover(is_output));
  fo->set_retry_interval(cfg.retry_interval);
  fo->set_endpoint(endp);
  fo->set_failover(failovr);
  return (fo.take());
}

/**
 *  Diff the current configuration with the new configuration.
 *
 *  @param[in]  current       Current endpoints.
 *  @param[in]  new_endpoints New endpoints configuration.
 *  @param[out] to_create     Endpoints that should be created.
 */
void endpoint::_diff_endpoints(QMap<config::endpoint, processing::failover*> & current,
                               QList<config::endpoint> const& new_endpoints,
                               QList<config::endpoint>& to_create) {
  // Find which endpoints will be kept, created and deleted.
  QMap<config::endpoint, processing::failover*> to_delete(current);
  for (QList<config::endpoint>::const_iterator it = new_endpoints.begin(),
         end = new_endpoints.end();
       it != end;
       ++it) {
    QMap<config::endpoint, processing::failover*>::iterator endp(to_delete.find(*it));
    if (endp != to_delete.end())
      to_delete.erase(endp);
    else
      to_create.push_back(*it);
  }

  // Remove old endpoints.
  for (QMap<config::endpoint, processing::failover*>::iterator it = to_delete.begin(),
         end = to_delete.end();
       it != end;
       ++it) {
    // Send only termination request, object will be destroyed by event
    // loop on termination. But wait for threads because they hold
    // resources that might be used by other endpoints.
    (*it)->exit();
    (*it)->wait();
  }

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Apply the endpoint configuration.
 *
 *  @param[in] inputs  Inputs configuration.
 *  @param[in] outputs Outputs configuration.
 */
void endpoint::apply(QList<config::endpoint> const& inputs,
                     QList<config::endpoint> const& outputs) {
  // Log messages.
  logging::config << logging::HIGH
    << "endpoint applier: loading configuration";
  logging::debug << logging::HIGH << "endpoint applier: "
    << inputs.size() << " inputs and "
    << outputs.size() << " outputs to apply";

  // Remove old inputs and generate inputs to create.
  QList<config::endpoint> in_to_create;
  {
    QMutexLocker lock(&_inputsm);
    _diff_endpoints(_inputs, inputs, in_to_create);
  }

  // Remove old outputs and generate outputs to create.
  QList<config::endpoint> out_to_create;
  {
    QMutexLocker lock(&_outputsm);
    _diff_endpoints(_outputs, outputs, out_to_create);
  }

  // Debug message.
  logging::debug << logging::HIGH << "endpoint applier: "
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
      QScopedPointer<processing::failover> endp(_create_endpoint(*it, false, true, out_to_create));
      connect(endp.data(), SIGNAL(finished()), this, SLOT(terminated_output()));
      connect(endp.data(), SIGNAL(terminated()), this, SLOT(terminated_output()));
      connect(endp.data(), SIGNAL(finished()), endp.data(), SLOT(deleteLater()));
      {
        QMutexLocker lock(&_outputsm);
        _outputs[*it] = endp.data();
      }

      // Run thread.
      logging::debug << logging::MEDIUM
        << "endpoint applier: output thread 0x"
       << QString().setNum((qulonglong)static_cast<QObject*>(endp.data()), 16)
        << " is registered and ready to run";
      endp.take()->start();
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
      QScopedPointer<processing::failover> endp(_create_endpoint(*it, true, false, in_to_create));
      connect(endp.data(), SIGNAL(finished()), this, SLOT(terminated_input()));
      connect(endp.data(), SIGNAL(terminated()), this, SLOT(terminated_input()));
      connect(endp.data(), SIGNAL(finished()), endp.data(), SLOT(deleteLater()));
      {
        QMutexLocker lock(&_inputsm);
        _inputs[*it] = endp.data();
      }

      // Run thread.
      logging::debug << logging::MEDIUM
        << "endpoint applier: input thread 0x"
        << QString().setNum((qulonglong)static_cast<QObject*>(endp.data()), 16)
        << " is registered and ready to run";
      endp.take()->start();
    }    

  return ;
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
endpoint& endpoint::instance() {
  static endpoint gl_endpoint;
  return (gl_endpoint);
}

/**
 *  An input thread finished executing.
 */
void endpoint::terminated_input() {
  QObject* sendr(QObject::sender());
  logging::debug << logging::MEDIUM
    << "endpoint applier: input thread 0x"
    << QString().setNum((qulonglong)sendr, 16)
    << " terminated";
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
  logging::debug << logging::MEDIUM
    << "endpoint applier: output thread 0x"
    << QString().setNum((qulonglong)sendr, 16)
    << " terminated";
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
 *  Unload the singleton.
 */
void endpoint::unload() {
  logging::debug << logging::HIGH << "endpoint applier: destruction";

  // Exit input threads.
  {
    logging::debug << logging::MEDIUM << "endpoint applier: " \
      "requesting input threads termination";
    QMutexLocker lock(&_inputsm);

    // Send termination requests.
    for (QMap<config::endpoint, processing::failover*>::iterator
           it = _inputs.begin(),
           end = _inputs.end();
         it != end;
         ++it)
      (*it)->exit();

    // Wait for threads.
    while (!_inputs.empty()) {
      logging::debug << logging::LOW << "endpoint applier: "
        << _inputs.size() << " input threads remaining";
      lock.unlock();
      time_t now(time(NULL));
      do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      } while (time(NULL) <= now);
      lock.relock();
    }
    logging::debug << logging::MEDIUM << "endpoint applier: all " \
      "input threads are terminated";
    _inputs.clear();
  }

  // Exit output threads.
  {
    logging::debug << logging::MEDIUM << "endpoint applier: " \
      "requesting output threads termination";
    QMutexLocker lock(&_outputsm);

    // Send termination requests.
    for (QMap<config::endpoint, processing::failover*>::iterator
           it = _outputs.begin(),
           end = _outputs.end();
         it != end;
         ++it)
      (*it)->exit();

    // Wait for threads.
    while (!_outputs.empty()) {
      logging::debug << logging::LOW << "endpoint applier: "
        << _outputs.size() << " output threads remaining";
      lock.unlock();
      time_t now(time(NULL));
      do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      } while (time(NULL) <= now);
      lock.relock();
    }
    logging::debug << logging::MEDIUM << "endpoint applier :all " \
      "output threads are terminated";
    _outputs.clear();
  }
}
