/*
** Copyright 2013 Centreon
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
  return *this;
}

/**
 *  Clone this object.
 *
 *  @return A Copy of this object.
 */
io::factory* factory::clone() const {
  return new factory(*this);
}

/**
 *  Check if endpoint configuration match the TLS layer.
 *
 *  @param[in] cfg  Configuration object.
 *
 *  @return True if the configuration matches the TLS layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find("tls")};
  return cfg.params.end() != it && strcasecmp(it->second.c_str(), "auto") &&
          config::parser::parse_boolean(it->second);
}

/**
 *  Check if endpoint configuration do not match the TLS layer.
 *
 *  @param[in] cfg  Configuration object.
 *
 *  @return True if the configuration does not match the TLS layer.
 */
bool factory::has_not_endpoint(config::endpoint& cfg) const {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find("tls")};
  return (it != cfg.params.end()
      && strcasecmp(it->second.c_str(), "auto"))
          ? !has_endpoint(cfg)
          : false;
}

/**
 *  Create an endpoint matching the configuration object.
 *
 *  @param[in] cfg         Configuration object.
 *  @param[in] is_acceptor Is true if endpoint is an acceptor, false
 *                         otherwise.
 *  @param[in] cache       Unused.
 *
 *  @return New endpoint object.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find TLS parameters (optional).
  bool tls(false);
  std::string ca_cert;
  std::string private_key;
  std::string public_cert;
  {
    // Is TLS enabled ?
    std::map<std::string, std::string>::const_iterator it{cfg.params.find("tls")};
    if (it != cfg.params.end()) {
      tls = config::parser::parse_boolean(it->second);
      if (tls) {
        // CA certificate.
        it = cfg.params.find("ca_certificate");
        if (it != cfg.params.end())
          ca_cert = it->second;

        // Private key.
        it = cfg.params.find("private_key");
        if (it != cfg.params.end())
          private_key = it->second;

        // Public certificate.
        it = cfg.params.find("public_cert");
        if (it != cfg.params.end())
          public_cert = it->second;
      }
    }
  }

  // Acceptor.
  std::unique_ptr<io::endpoint> endp;
  if (is_acceptor)
    endp.reset(new acceptor(public_cert, private_key, ca_cert));
  // Connector.
  else
    endp.reset(new connector(public_cert, private_key, ca_cert));
  return endp.release();
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
std::shared_ptr<io::stream> factory::new_stream(
                                        std::shared_ptr<io::stream> to,
                                        bool is_acceptor,
                                        std::string const& proto_name) {
  (void)proto_name;
  return is_acceptor ? acceptor().open(to) : connector().open(to);
}
