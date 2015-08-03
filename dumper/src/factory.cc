/*
** Copyright 2013,2015 Merethis
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
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/dumper/factory.hh"
#include "com/centreon/broker/dumper/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static QString const& find_param(
                        config::endpoint const& cfg,
                        QString const& key) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    throw (exceptions::msg() << "dumper: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return (it.value());
}

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
 *  @param[in] other  Object to copy.
 */
factory::factory(factory const& other) : io::factory(other) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& other) {
  io::factory::operator=(other);
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
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches the dumper layer.
 */
bool factory::has_endpoint(config::endpoint& cfg,
                           bool is_input,
                           bool is_output) const {
  return (is_output &&
          (cfg.type == "dumper"
           || cfg.type == "db_cfg_reader"
           || cfg.type == "db_cfg_writer"));
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Persistent cache.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  (void) is_input;
  (void) is_output;
  // Set acceptor flag.
  is_acceptor = false;

  // Get the type of this dumper.
  opener::dumper_type type = opener::dump;
  if (cfg.type == "dump")
    type = opener::dump;
  else if (cfg.type == "db_cfg_reader")
    type = opener::db_cfg_reader;
  else if (cfg.type == "db_cfg_writer")
    type = opener::db_cfg_writer;

  // Opener that should be set.
  std::auto_ptr<opener> openr(new opener);
  openr->set_name(cfg.name.toStdString());
  openr->set_type(type);

  // DB configuration dumpers.
  if ((type == opener::db_cfg_reader)
      || (type == opener::db_cfg_writer)) {
    // DB parameters.
    std::string db_type(find_param(cfg, "db_type").toStdString());
    std::string host(find_param(cfg, "db_host").toStdString());
    unsigned short port(find_param(cfg, "db_port").toUInt());
    std::string user(find_param(cfg, "db_user").toStdString());
    std::string password(find_param(cfg, "db_password").toStdString());
    std::string db_name(find_param(cfg, "db_name").toStdString());

    // Set opener properties.
    openr->set_db(database_config(
                    db_type,
                    host,
                    port,
                    user,
                    password,
                    db_name));
  }
  // Filesystem dumpers.
  else {
    // Find path to the dumper.
    std::string path(find_param(cfg, "path").toStdString());

    // Find tagname to the dumper.
    std::string tagname(find_param(cfg, "tagname").toStdString());

    // Set opener properties.
    openr->set_path(path);
    openr->set_tagname(tagname);
  }

  return (openr.release());
}
