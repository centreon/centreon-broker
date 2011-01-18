/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <memory>
#include "config/factory.hh"
#include "config/interface.hh"
#include "config/logger.hh"
#include "interface/db/destination.hh"
#include "interface/ndo/destination.hh"
#include "interface/ndo/source.hh"
#include "interface/ndo/source_destination.hh"
#include "interface/xml/destination.hh"
#include "io/net/ipv4.hh"
#include "io/net/ipv6.hh"
#include "io/net/unix.hh"
#include "io/split.hh"
#ifdef USE_TLS
# include "io/tls/acceptor.hh"
# include "io/tls/connector.hh"
#endif /* USE_TLS */

using namespace interface;

/**************************************
*                                     *
*            Local Methods            *
*                                     *
**************************************/

#ifdef USE_TLS
/**
 *  Wraps a Stream within a TLS layer.
 *
 *  @param[in] i Interface configuration.
 *  @param[in] s Stream on which TLS will be applied.
 *
 *  @return The stream object wrapped in a TLS object.
 */
static io::stream* build_tls_connector(config::interface const& i,
                                       io::stream* s) {
  std::auto_ptr<io::stream> tmp(s);
  std::auto_ptr<io::tls::connector> connector(new io::tls::connector(s));
  tmp.release();
  if (!i.cert.empty())
    connector->set_cert(i.cert, i.key);
  if (!i.ca.empty())
    connector->set_trusted_ca(i.ca);
  if (i.compress)
    connector->set_compression(true);
  connector->connect();
  return (connector.release());
}
#endif /* USE_TLS */

/**
 *  Build a file according to the configuration provided.
 *
 *  @param[in] i Configuration of the file.
 *
 *  @return An open file.
 */
static io::stream* build_file(config::interface const& i) {
  std::auto_ptr<io::split> split(new io::split);
  split->base_file(i.filename);
  return (split.release());
}

/**
 *  Build an IPv4 connector according to the configuration provided.
 *
 *  @param[in] i Configuration of the IPv4 connector.
 *
 *  @return A connected IPv4 connector.
 */
static io::stream* build_ipv4_connector(config::interface const& i) {
  std::auto_ptr<io::net::ipv4_connector> ipv4c(new io::net::ipv4_connector);
  ipv4c->connect(i.host.c_str(), i.port);
  return (
#ifdef USE_TLS
          i.tls ? build_tls_connector(i, ipv4c.release()) :
#endif /* USE_TLS */
                  ipv4c.release());
}

/**
 *  Build an IPv6 connector according to the configuration provided.
 *
 *  @param[in] i Configuration of the IPv6 connector.
 *
 *  @return A connected IPv6 connector.
 */
static io::stream* build_ipv6_connector(config::interface const& i) {
  std::auto_ptr<io::net::ipv6_connector> ipv6c(new io::net::ipv6_connector);
  ipv6c->connect(i.host.c_str(), i.port);
  return (
#ifdef USE_TLS
          i.tls ? build_tls_connector(i, ipv6c.release()) :
#endif /* USE_TLS */
                  ipv6c.release());
}

/**
 *  Build a Unix connector according to the configuration provided.
 *
 *  @param[in] i Configuration of the Unix connector.
 *
 *  @return A connected Unix connector.
 */
static io::stream* build_unix_connector(config::interface const& i) {
  std::auto_ptr<io::net::unix_connector> uc(new io::net::unix_connector);
  uc->connect(i.socket.c_str());
  return (
#ifdef USE_TLS
          i.tls ? build_tls_connector(i, uc.release()) :
#endif /* USE_TLS */
                  uc.release());
}

static io::stream* build_stream(config::interface const& i) {
  std::auto_ptr<io::stream> stream;
  switch (i.type) {
   case config::interface::file:
    stream.reset(build_file(i));
    break ;
   case config::interface::ipv4_client:
    stream.reset(build_ipv4_connector(i));
    break ;
   case config::interface::ipv6_client:
    stream.reset(build_ipv6_connector(i));
    break ;
   case config::interface::unix_client:
    stream.reset(build_unix_connector(i));
    break ;
   default:
    ;
  }
  return (stream.release());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Build an acceptor from its configuration.
 *
 *  @param[in] i Configuration of the new acceptor.
 *
 *  @return An acceptor matching the configuration.
 */
io::acceptor* config::factory::build_acceptor(config::interface const& i) {
  std::auto_ptr<io::acceptor> acceptor;
  switch (i.type) {
   case config::interface::ipv4_server:
    {
      std::auto_ptr<io::net::ipv4_acceptor> ipv4a(new io::net::ipv4_acceptor);
      if (i.net_iface.empty())
        ipv4a->listen(i.port);
      else
        ipv4a->listen(i.port, i.net_iface.c_str());
      acceptor.reset(ipv4a.release());
    }
    break ;
   case config::interface::ipv6_server:
    {
      std::auto_ptr<io::net::ipv6_acceptor> ipv6a(new io::net::ipv6_acceptor);
      if (i.net_iface.empty())
        ipv6a->listen(i.port);
      else
        ipv6a->listen(i.port, i.net_iface.c_str());
      acceptor.reset(ipv6a.release());
    }
    break ;
   case config::interface::unix_server:
    {
      std::auto_ptr<io::net::unix_acceptor> ua(new io::net::unix_acceptor);
      ua->listen(i.socket.c_str());
      acceptor.reset(ua.release());
    }
    break ;
   default:
    ;
  }
#ifdef USE_TLS
  if (i.tls) {
    std::auto_ptr<io::tls::acceptor> tlsa(new io::tls::acceptor);
    tlsa->listen(acceptor.get());
    acceptor.release();
    acceptor.reset(tlsa.release());
  }
#endif /* !USE_TLS */
  return (acceptor.release());
}

/**
 *  Build a destination from its configuration.
 *
 *  @param[in] i Configuration of the new destination.
 *
 *  @return A destination matching the configuration.
 */
interface::destination* config::factory::build_destination(config::interface const& i) {
  ::interface::destination* dest;
  switch (i.type) {
   case config::interface::file:
    {
      std::auto_ptr<io::split> split(new io::split);
      // XXX : set file num + file size
      split->base_file(i.filename);
      if (config::interface::xml == i.protocol)
        dest = new ::interface::xml::destination(split.get());
      else
        dest = new ::interface::ndo::destination(split.get());
      split.release();
    }
    break ;
   case config::interface::ipv4_client:
    {
      std::auto_ptr<io::stream> ipv4c(build_ipv4_connector(i));
      if (config::interface::xml == i.protocol)
        dest = new ::interface::xml::destination(ipv4c.get());
      else
        dest = new ::interface::ndo::destination(ipv4c.get());
      ipv4c.release();
    }
    break ;
   case config::interface::ipv6_client:
    {
      std::auto_ptr<io::stream> ipv6c(build_ipv6_connector(i));
      if (config::interface::xml == i.protocol)
        dest = new ::interface::xml::destination(ipv6c.get());
      else
        dest = new ::interface::ndo::destination(ipv6c.get());
      ipv6c.release();
    }
    break ;
#ifdef USE_MYSQL
   case config::interface::mysql:
    {
      std::auto_ptr<::interface::db::destination> db(
        new ::interface::db::destination);
      db->connect(::interface::db::destination::MYSQL,
                  i.db,
                  i.host,
                  i.user,
                  i.password);
      dest = db.release();
    }
    break ;
#endif /* USE_MYSQL */
#ifdef USE_ORACLE
   case config::interface::oracle:
    {
      std::auto_ptr<::interface::db::destination> db(
        new ::interface::db::destination);
    db->connect(::interface::db::destination::ORACLE,
                  i.db,
                  i.host,
                  i.user,
                  i.password);
      dest = db.release();
    }
    break ;
#endif /* USE_ORACLE */
#ifdef USE_POSTGRESQL
   case config::interface::postgresql:
    {
      std::auto_ptr<::interface::db::destination> db(
        new ::interface::db::destination);
      db->connect(::interface::db::destination::POSTGRESQL,
                  i.db,
                  i.host,
                  i.user,
                  i.password);
      dest = db.release();
    }
    break ;
#endif /* USE_POSTGRESQL */
   case config::interface::unix_client:
    {
      std::auto_ptr<io::stream> uc(build_unix_connector(i));
      if (config::interface::xml == i.protocol)
        dest = new ::interface::xml::destination(uc.get());
      else
        dest = new ::interface::ndo::destination(uc.get());
      uc.release();
    }
    break ;
   default:
    dest = NULL;
  }

  return (dest);
}

/**
 *  Build a source from its configuration.
 *  @par Safety Strong exception safety.
 *
 *  @param[in] i Configuration of the new destination.
 *
 *  @return A source matching the configuration.
 */
interface::source* config::factory::build_source(config::interface const& i) {
  std::auto_ptr< ::interface::ndo::source> source;
  std::auto_ptr<io::stream> stream;
  stream.reset(build_stream(i));
  source.reset(new ::interface::ndo::source(stream.get()));
  stream.release();
  return (source.release());
}

/**
 *  Build a source-destination from its configuration.
 *
 *  @param[in] i Configuration of the new destination.
 *
 *  @return A source-destination matching the configuration.
 */
interface::source_destination* config::factory::build_sourcedestination(config::interface const& i) {
  std::auto_ptr< ::interface::source_destination> sd;
  std::auto_ptr<io::stream> stream(build_stream(i));
  if (stream.get()) {
    sd.reset(new ndo::source_destination(stream.get()));
    stream.release();
  }
  return (sd.release());
}
