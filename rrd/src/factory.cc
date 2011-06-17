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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/rrd/connector.hh"
#include "com/centreon/broker/rrd/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

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
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) : io::factory(f) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration match the RRD layer.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if endpoint should act as input.
 *  @param[in] is_output true if endpoint should act as output.
 *
 *  @return true if the configuration matches the RRD layer.
 */
bool factory::has_endpoint(config::endpoint const& cfg, 
                           bool is_input,
                           bool is_output) const {
  (void)is_output;
  return (!is_input && (cfg.type == "rrd"));
}

/**
 *  Build a RRD endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if endpoint should act as input.
 *  @param[in]  is_output   true if endpoint should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *
 *  @return Endpoint matching the given configuration.
 */
io::endpoint* factory::new_endpoint(config::endpoint const& cfg,
                                    bool is_input,
                                    bool is_output,
                                    bool& is_acceptor) const {
  (void)is_output;

  // Check that endpoint is output only.
  if (is_input)
    throw (exceptions::msg() << "cannot create an input RRD endpoint");

  // Get metrics RRD path.
  QMap<QString, QString>::const_iterator it1(cfg.params.find("metrics_path"));
  if (it1 == cfg.params.end())
    throw (exceptions::msg() << "no 'metrics_path' defined for RRD endpoint '"
             << cfg.name.toStdString().c_str() << "'");

  // Get status RRD path.
  QMap<QString, QString>::const_iterator it2(cfg.params.find("status_path"));
  if (it2 == cfg.params.end())
    throw (exceptions::msg() << "no 'status_path' defined for RRD endpoint '"
             << cfg.name.toStdString().c_str() << "'");

  // Create endpoint.
  QScopedPointer<rrd::connector> endp(new rrd::connector);
  endp->set_metrics_path(it1.value());
  endp->set_status_path(it2.value());
  is_acceptor = false;
  return (endp.take());
}
