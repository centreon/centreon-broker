/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/connector.hh"
#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
factory::factory(factory const& right) : io::factory(right) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& right) {
  io::factory::operator=(right);
  return (*this);
}

/**
 *  Clone the factory object.
 *
 *  @return Clone of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  @brief Check if a configuration supports this protocol.
 *
 *  The endpoint 'protocol' tag must have the 'bbdo' value.
 *
 *  @param[in] cfg       Object configuration.
 *  @param[in] is_input  true if endpoint must act as event source.
 *  @param[in] is_output true if endpoint must act as event destination.
 *
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  QMap<QString, QString>::const_iterator it(cfg.params.find("protocol"));
  return ((it != cfg.params.end()) && (it.value() == "bbdo"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if endpoint must act as event source.
 *  @param[in]  is_output   true if endpoint must act as event
 *                          destination.
 *  @param[in]  temporary   Temporary stream to write data when memory
 *                          queue is full.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         io::endpoint const* temporary,
                         bool& is_acceptor) const {
  // Return value.
  io::endpoint* retval(NULL);

  // Negociation allowed ?
  bool negociate;
  QString extensions;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("negociation"));
    if ((it != cfg.params.end())
        && (*it == "no"))
      negociate = false;
    else {
      negociate = true;
      extensions = _extensions(cfg, is_input, is_output);
    }
  }

  // Create object.
  if (is_acceptor)
    retval = new bbdo::acceptor(
                         is_output,
                         negociate,
                         extensions,
                         temporary);
  else
    retval = new bbdo::connector(
                         is_input,
                         is_output,
                         negociate,
                         extensions);
  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Get available extensions for an endpoint.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  Input flag.
 *  @param[in] is_output Output flag.
 */
QString factory::_extensions(
                   config::endpoint& cfg,
                   bool is_input,
                   bool is_output) const {
  QString extensions;
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it->osi_from > 1)
        && (it->osi_to < 7)
        && !it->endpntfactry->has_endpoint(
                                cfg,
                                is_input,
                                is_output)
        && !it->endpntfactry->has_not_endpoint(
                                cfg,
                                is_input,
                                is_output)) {
      if (!extensions.isEmpty())
        extensions.append(" ");
      extensions.append(it.key());
    }
  }
  return (extensions);
}
