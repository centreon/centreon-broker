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

#include <assert.h>
#include <stdlib.h>
#include "config/applier/endpoint.hh"
#include "exceptions/basic.hh"
#include "io/acceptor.hh"
#include "io/connector.hh"
#include "io/protocols.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

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
 *  @param[in] is_output true if the endpoint will act as output.
 */
void endpoint::_create_endpoint(config::endpoint const& cfg, bool is_output) {
  // Create endpoint object.
  QSharedPointer<io::endpoint> endp;
  bool is_acceptor;
  int level;
  for (QMap<QString, io::protocols::protocol>::const_iterator it = io::protocols::instance().begin(),
         end = io::protocols::instance().end();
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg, !is_output, is_output)) {
      endp = QSharedPointer<io::endpoint>(it.value().endpntfactry->new_endpoint(cfg, !is_output, is_output, is_acceptor));
      level = it.value().osi_to + 1;
      break ;
    }
  }
  if (endp.isNull())
    throw (exceptions::basic() << "no matching protocol found for endpoint '"
             << cfg.name.toStdString().c_str() << "'");

  // Create remaining objects.
  io::endpoint* prev(endp.data());
  while (level <= 7) {
    // Browse protocol list.
    QMap<QString, io::protocols::protocol>::const_iterator it(io::protocols::instance().begin());
    QMap<QString, io::protocols::protocol>::const_iterator end(io::protocols::instance().end());
    while (it != end) {
      if ((it.value().osi_from == level)
          && (it.value().endpntfactry->has_endpoint(cfg, !is_output, is_output))) {
        if (is_acceptor) {
          QSharedPointer<io::acceptor> current(static_cast<io::acceptor*>(it.value().endpntfactry->new_endpoint(cfg, !is_output, is_output, is_acceptor)));
          static_cast<io::acceptor*>(prev)->on(current);
          prev = current.data();
        }
        else {
          QSharedPointer<io::connector> current(static_cast<io::connector*>(it.value().endpntfactry->new_endpoint(cfg, !is_output, is_output, is_acceptor)));
          static_cast<io::connector*>(prev)->on(current);
          prev = current.data();
        }
        level = it.value().osi_to;
        break ;
      }
      ++it;
    }
    if ((7 == level) && (it == end))
      throw (exceptions::basic() << "no matching protocol found for endpoint '"
               << cfg.name.toStdString().c_str() << "'");
    ++level;
  }

  // Create thread.
  QScopedPointer<processing::failover> fo(new processing::failover);
  fo->set_endpoint(endp,
    (is_output
       ? processing::failover::output
       : processing::failover::input));

  // Connect thread.
  connect(fo.data(), SIGNAL(finished()), fo.data(), SLOT(deleteLater()));
  if (!is_output) {
    connect(fo.data(), SIGNAL(finished()), this, SLOT(terminated_input()));
    _inputs[cfg] = fo.data();
  }
  else {
    connect(fo.data(), SIGNAL(finished()), this, SLOT(terminated_output()));
    _outputs[cfg] = fo.data();
  }

  // Run thread.
  fo.take()->start();

  return ;
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
    // XXX : send only termination request, object will
    //       be destroyed by event loop on termination.
    //       But wait for threads, because 
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
  // Remove old inputs and generate inputs to create.
  QList<config::endpoint> in_to_create;
  _diff_endpoints(_inputs, inputs, in_to_create);

  // Remove old outputs and generate outputs to create.
  QList<config::endpoint> out_to_create;
  _diff_endpoints(_outputs, outputs, out_to_create);

  // Create new outputs.
  for (QList<config::endpoint>::iterator it = out_to_create.begin(),
         end = out_to_create.end();
       it != end;
       ++it)
    _create_endpoint(*it, true);

  // Create new inputs.
  for (QList<config::endpoint>::iterator it = in_to_create.begin(),
         end = in_to_create.end();
       it != end;
       ++it)
    _create_endpoint(*it, false);

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
  // XXX
}

/**
 *  An output thread finished executing.
 */
void endpoint::terminated_output() {
  // XXX
}
