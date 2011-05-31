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
#include "exceptions/basic.hh"
#include "sql/connector.hh"
#include "sql/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

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
 *  Clone the factory.
 *
 *  @return Copy of the factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint should be an input.
 *  @param[in] is_output true if the endpoint should be an output.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint const& cfg,
                           bool is_input,
                           bool is_output) const {
  (void)is_output;
  QMap<QString, QString>::const_iterator it(cfg.params.find("db"));
  return (!is_input && (it != cfg.params.end()));
}

/**
 *  Create an endpoint.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint should be an input.
 *  @param[in] is_output true if the endpoint should be an output.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(config::endpoint const& cfg,
                                    bool is_input,
                                    bool is_output,
                                    bool& is_acceptor) const {
  (void)is_output;
  if (is_input)
    throw (exceptions::basic() << "SQL layer cannot act as input object");

  // Find DB host.
  QString host;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("host"));
    if (it == cfg.params.end())
      throw (exceptions::basic() << "no 'host' defined for SQL endpoint '"
               << cfg.name.toStdString().c_str() << "'");
    host = it.value();
  }

  // Find DB user.
  QString user;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("user"));
    if (it == cfg.params.end())
      throw (exceptions::basic() << "no 'user' defined for SQL endpoint '"
               << cfg.name.toStdString().c_str() << "'");
    user = it.value();
  }

  // Find DB password.
  QString password;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("password"));
    if (it == cfg.params.end())
      throw (exceptions::basic() << "no 'password' defined for SQL endpoint '"
               << cfg.name.toStdString().c_str() << "'");
    password = it.value();
  }

  // Find DB name.
  QString name;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("db"));
    if (it == cfg.params.end())
      throw (exceptions::basic() << "no 'db' defined for SQL endpoint '"
               << cfg.name.toStdString().c_str() << "'");
    name = it.value();
  }

  // Connector.
  QScopedPointer<sql::connector> c(new sql::connector);
  c->connect_to(cfg.type, host, user, password, name);
  is_acceptor = false;
  return (c.take());
}
