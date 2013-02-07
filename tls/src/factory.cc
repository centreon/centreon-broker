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
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/tls/acceptor.hh"
#include "com/centreon/broker/tls/connector.hh"
#include "com/centreon/broker/tls/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;

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
 *  Clone this object.
 *
 *  @return A Copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if endpoint configuration match the TLS layer.
 *
 *  @param[in] cfg       Configuration object.
 *  @param[in] is_input  Unused.
 *  @param[in] is_output Unused.
 *
 *  @return true if the configuration matches the TLS layer.
 */
bool factory::has_endpoint(
                config::endpoint const& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("tls"));
  return ((cfg.params.end() != it)
          && config::parser::parse_boolean(*it));
}

/**
 *  Create an endpoint matching the configuration object.
 *
 *  @param[in] cfg         Configuration object.
 *  @param[in] is_input    true if the endpoint should be an input
 *                         object.
 *  @param[in] is_output   true if the endpoint should be an output
 *                         object.
 *  @param[in] temporary   Unused.
 *  @param[in] is_acceptor Is true if endpoint is an acceptor, false
 *                         otherwise.
 *
 *  @return New endpoint object.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         io::endpoint const* temporary,
                         bool& is_acceptor) const {
  (void)is_input;
  (void)is_output;
  (void)temporary;

  // Find TLS parameters (optional).
  bool tls(false);
  std::string ca_cert;
  std::string private_key;
  std::string public_cert;
  {
    // Is TLS enabled ?
    QMap<QString, QString>::const_iterator it(cfg.params.find("tls"));
    if (it != cfg.params.end()) {
      tls = config::parser::parse_boolean(*it);
      if (tls) {
        // CA certificate.
        it = cfg.params.find("ca_certificate");
        if (it != cfg.params.end())
          ca_cert = it.value().toStdString();

        // Private key.
        it = cfg.params.find("private_key");
        if (it != cfg.params.end())
          private_key = it.value().toStdString();

        // Public certificate.
        it = cfg.params.find("public_cert");
        if (it != cfg.params.end())
          public_cert = it.value().toStdString();
      }
    }
  }

  // Acceptor.
  std::auto_ptr<io::endpoint> endp;
  if (is_acceptor)
    endp.reset(new acceptor(public_cert, private_key, ca_cert));
  // Connector.
  else
    endp.reset(new connector(public_cert, private_key, ca_cert));
  return (endp.release());
}

/**
 *  Get new TLS stream.
 *
 *  @param[in] to          Lower stream.
 *  @param[in] is_acceptor true if 'to' is an acceptor.
 *  @param[in] proto_name  Unused.
 *
 *  @return New stream.
 */
misc::shared_ptr<io::stream> factory::new_stream(
                                        misc::shared_ptr<io::stream> to,
                                        bool is_acceptor,
                                        QString const& proto_name) {
  (void)proto_name;
  return (is_acceptor ? acceptor().open(to) : connector().open(to));
}
