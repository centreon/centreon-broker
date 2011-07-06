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

#include <QScopedPointer>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/factory.hh"
#include "com/centreon/broker/file/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

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
 *  @return Exact copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration match the file layer.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the file should act as input.
 *  @param[in] is_output true if the file should act as output.
 *
 *  @return true if configuration matches the file layer.
 */
bool factory::has_endpoint(config::endpoint const& cfg,
                           bool is_input,
                           bool is_output) const {
  (void)is_input;
  (void)is_output;
  return (cfg.type == "file");
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the file should act as input.
 *  @param[in]  is_output   true if the file should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(config::endpoint const& cfg,
                                    bool is_input,
                                    bool is_output,
                                    bool& is_acceptor) const {
  (void)is_acceptor;

  // Find path to the file.
  QMap<QString, QString>::const_iterator it(cfg.params.find("path"));
  if (it == cfg.params.end())
    throw (exceptions::msg() << "file: no 'path' defined for file " \
             "endpoint '" << cfg.name << "'");

  // Generate opener.
  QScopedPointer<opener> openr(new opener(is_input, is_output));
  openr->set_filename(it.value());
  return (openr.take());
}
