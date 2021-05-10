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

#include "com/centreon/broker/tls/factory.hh"

#include <cstring>
#include <memory>

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/acceptor.hh"
#include "com/centreon/broker/tls/connector.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;

/**
 *  Check if an endpoint configuration match the TLS layer.
 *
 *  @param[in] cfg  Configuration object.
 *  @param[out] extension Returns no, maybe or yes with options to give the
 *              tls configuration.
 *
 *  @return False everytime because the TLS layer must not be set at
 *  the broker configuration. This avoids the TLS while the negotiation
 *  is running. We will be able to add this endpoint later, following the ext
 *  value.
 */
bool factory::has_endpoint(config::endpoint& cfg, io::extension* ext) {
  if (ext) {
    auto it = cfg.params.find("tls");
    if (it == cfg.params.end() || strncasecmp(it->second.c_str(), "no", 3) == 0)
      *ext = io::extension("TLS", false, false);
    else {
      log_v2::tls()->info("Configuration of TLS for endpoint '{}'",
                          cfg.params["name"]);
      if (strncasecmp(it->second.c_str(), "auto", 5) == 0)
        *ext = io::extension("TLS", true, false);
      else if (strncasecmp(it->second.c_str(), "yes", 4) == 0)
        *ext = io::extension("TLS", false, true);

      // CA certificate.
      it = cfg.params.find("ca_certificate");
      if (it != cfg.params.end())
        ext->mutable_options()["ca_cert"] = it->second;

      // Private key.
      it = cfg.params.find("private_key");
      if (it != cfg.params.end())
        ext->mutable_options()["private_key"] = it->second;

      // Public certificate.
      it = cfg.params.find("public_cert");
      if (it != cfg.params.end())
        ext->mutable_options()["public_cert"] = it->second;

      // tls hostname.
      it = cfg.params.find("tls_hostname");
      if (it != cfg.params.end())
        ext->mutable_options()["tls_hostname"] = it->second;
    }
  }
  return false;
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
  bool tls{false};
  std::string private_key;
  std::string public_cert;
  std::string ca_cert;
  std::string tls_hostname;
  {
    // Is TLS enabled ?
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("tls")};
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

        // tls hostname.
        it = cfg.params.find("tls_hostname");
        if (it != cfg.params.end())
          tls_hostname = it->second;
      }
    }
  }

  // Acceptor.
  std::unique_ptr<io::endpoint> endp;
  if (is_acceptor)
    endp.reset(new acceptor(public_cert, private_key, ca_cert, tls_hostname));
  // Connector.
  else
    endp.reset(new connector(public_cert, private_key, ca_cert, tls_hostname));
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
    const std::unordered_map<std::string, std::string>& options) {
  std::string public_cert;
  std::string private_key;
  std::string ca_cert;
  std::string tls_hostname;

  auto found = options.find("public_cert");
  if (found != options.end())
    public_cert = found->second;

  found = options.find("private_key");
  if (found != options.end())
    private_key = found->second;

  found = options.find("ca_cert");
  if (found != options.end())
    ca_cert = found->second;

  found = options.find("tls_hostname");
  if (found != options.end())
    tls_hostname = found->second;

  return is_acceptor
             ? acceptor(public_cert, private_key, ca_cert, tls_hostname)
                   .open(to)
             : connector(public_cert, private_key, ca_cert, tls_hostname)
                   .open(to);
}
