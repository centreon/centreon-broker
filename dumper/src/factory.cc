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

#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/dumper/factory.hh"
#include "com/centreon/broker/dumper/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

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
 *  Check if a configuration match the dumper layer.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the dumper should act as input.
 *  @param[in] is_output true if the dumper should act as output.
 *
 *  @return true if configuration matches the dumper layer.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  bool retval;
  if (cfg.type == "dumper") {
    cfg.params["coarse"] = "yes"; // Dumper won't respond to any salutation.
    retval = true;
  }
  else
    retval = false;
  return (retval);
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the dumper should act as input.
 *  @param[in]  is_output   true if the dumper should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)is_acceptor;

  // Find path to the dumper.
  std::string path;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("path"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "dumper: no 'path' defined for dumper "
             "endpoint '" << cfg.name << "'");
    path = it->toStdString();
  }

  // Find tagname to the dumper.
  std::string tagname;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("tagname"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "dumper: no 'tagname' defined for dumper "
             "endpoint '" << cfg.name << "'");
    tagname = it->toStdString();
  }

  // Generate opener.
  std::auto_ptr<opener> openr(new opener(is_input, is_output));
  openr->set_path(path);
  openr->set_tagname(tagname);
  return (openr.release());
}
