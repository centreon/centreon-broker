/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <memory>
#include <QString>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

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
 *  Clone the factory object.
 *
 *  @return Clone of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration supports this protocol.
 *
 *  @param[in] cfg       Object configuration.
 *  @param[in] is_input  Unused.
 *  @param[in] is_output Unused.
 *
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  return ((cfg.type == "ip")
          || (cfg.type == "tcp")
          || (cfg.type == "ipv4")
          || (cfg.type == "ipv6"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    Unused.
 *  @param[in]  is_output   Unused.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  // Find host (if exist).
  QString host;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("host"));
    if (it != cfg.params.end())
      host = it.value();
  }

  // Find port (must exist).
  unsigned short port;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("port"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "TCP: no 'port' defined for " \
               "endpoint '" << cfg.name << "'");
    port = it.value().toUShort();
  }

  // Find TCP socket timeout option.
  int socket_timeout(-1);
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("socket_write_timeout"));
    if (it != cfg.params.end())
      socket_timeout = it.value().toUInt();
  }

  // Acceptor.
  std::auto_ptr<io::endpoint> endp;
  if (host.isEmpty()) {
    is_acceptor = true;
    std::auto_ptr<tcp::acceptor> a(new tcp::acceptor);
    a->set_write_timeout(socket_timeout);
    a->listen_on(port);
    endp.reset(a.release());
  }
  // Connector.
  else {
    is_acceptor = false;
    std::auto_ptr<tcp::connector> c(new tcp::connector);
    c->connect_to(host, port);
    c->set_write_timeout(socket_timeout);
    c->set_read_timeout(is_input && is_output ? 30 : -1);
    endp.reset(c.release());
  }
  return (endp.release());
}
