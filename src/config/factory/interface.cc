/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <memory>
#include "config/factory.hh"
#include "interface/db/destination.h"
#include "interface/ndo/destination.h"
#include "interface/ndo/source.h"
#include "interface/ndo/sourcedestination.h"
#include "interface/xml/destination.h"
#include "io/net/ipv4.h"
#include "io/net/ipv6.h"
#include "io/net/unix.h"
#include "io/split.h"
#ifdef USE_TLS
# include "io/tls/acceptor.h"
# include "io/tls/connector.h"
#endif /* USE_TLS */

using namespace config;
using namespace Interface;

/**************************************
*                                     *
*            Local Methods            *
*                                     *
**************************************/

#ifdef USE_TLS
/**
 *  Wraps a Stream within a TLS layer.
 *
 *  @return The stream object wrapped in a TLS object.
 */
static IO::Stream* build_tls_connector(config::interface const& i,
                                       IO::Stream* stream)
{
  std::auto_ptr<IO::Stream> tmp(stream);
  std::auto_ptr<IO::TLS::Connector> connector(new IO::TLS::Connector(stream));

  tmp.release();
  if (!i.cert.empty())
    connector->SetCert(i.cert, i.key);
  if (!i.ca.empty())
    connector->SetTrustedCA(i.ca);
  if (i.compress)
    connector->SetCompression(true);
  connector->Connect();
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
static IO::Stream* build_file(config::interface const& i)
{
  std::auto_ptr<IO::Split> split(new IO::Split);

  split->BaseFile(i.filename);
  return (split.release());
}

/**
 *  Build an IPv4 connector according to the configuration provided.
 *
 *  @param[in] i Configuration of the IPv4 connector.
 *
 *  @return A connected IPv4 connector.
 */
static IO::Stream* build_ipv4_connector(config::interface const& i)
{
  std::auto_ptr<IO::Net::IPv4Connector> ipv4c(new IO::Net::IPv4Connector);

  ipv4c->Connect(i.host.c_str(), i.port);
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
static IO::Stream* build_ipv6_connector(config::interface const& i)
{
  std::auto_ptr<IO::Net::IPv6Connector> ipv6c(new IO::Net::IPv6Connector);

  ipv6c->Connect(i.host.c_str(), i.port);
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
static IO::Stream* build_unix_connector(config::interface const& i)
{
  std::auto_ptr<IO::Net::UnixConnector> uc(new IO::Net::UnixConnector);

  uc->Connect(i.socket.c_str());
  return (
#ifdef USE_TLS
          i.tls ? build_tls_connector(i, uc.release()) :
#endif /* USE_TLS */
                  uc.release());
}

static IO::Stream* build_stream(config::interface const& i)
{
  std::auto_ptr<IO::Stream> stream;

  switch (i.type)
    {
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
IO::Acceptor* factory::build_acceptor(config::interface const& i)
{
  std::auto_ptr<IO::Acceptor> acceptor;

  switch (i.type)
    {
     case config::interface::ipv4_server:
      {
        std::auto_ptr<IO::Net::IPv4Acceptor> ipv4a(new IO::Net::IPv4Acceptor);

        if (i.net_iface.empty())
          ipv4a->Listen(i.port);
        else
          ipv4a->Listen(i.port, i.net_iface.c_str());
        acceptor.reset(ipv4a.release());
      }
      break ;
     case config::interface::ipv6_server:
      {
        std::auto_ptr<IO::Net::IPv6Acceptor> ipv6a(new IO::Net::IPv6Acceptor);

        if (i.net_iface.empty())
          ipv6a->Listen(i.port);
        else
          ipv6a->Listen(i.port, i.net_iface.c_str());
        acceptor.reset(ipv6a.release());
      }
      break ;
     case config::interface::unix_server:
      {
        std::auto_ptr<IO::Net::UnixAcceptor> ua(new IO::Net::UnixAcceptor);

        ua->Listen(i.socket.c_str());
        acceptor.reset(ua.release());
      }
      break ;
     default:
      ;
    }
#ifdef USE_TLS
  if (i.tls)
    {
      std::auto_ptr<IO::TLS::Acceptor> tlsa(new IO::TLS::Acceptor);

      tlsa->Listen(acceptor.get());
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
Interface::Destination* factory::build_destination(config::interface const& i)
{
  Interface::Destination* dest;

  switch (i.type)
    {
     case config::interface::file:
      {
        std::auto_ptr<IO::Split> split(new IO::Split);

        // XXX : set file num + file size
        split->BaseFile(i.filename);
        if (config::interface::xml == i.protocol)
          dest = new Interface::XML::Destination(split.get());
        else
          dest = new Interface::NDO::Destination(split.get());
        split.release();
      }
      break ;
     case config::interface::ipv4_client:
      {
        std::auto_ptr<IO::Stream> ipv4c(build_ipv4_connector(i));

        if (config::interface::xml == i.protocol)
          dest = new Interface::XML::Destination(ipv4c.get());
        else
          dest = new Interface::NDO::Destination(ipv4c.get());
        ipv4c.release();
      }
      break ;
     case config::interface::ipv6_client:
      {
        std::auto_ptr<IO::Stream> ipv6c(build_ipv6_connector(i));

        if (config::interface::xml == i.protocol)
          dest = new Interface::XML::Destination(ipv6c.get());
        else
          dest = new Interface::NDO::Destination(ipv6c.get());
        ipv6c.release();
      }
      break ;
#ifdef USE_MYSQL
     case config::interface::mysql:
      {
        std::auto_ptr<Interface::DB::Destination> db(
          new Interface::DB::Destination);

        db->Connect(Interface::DB::Destination::MYSQL,
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
        std::auto_ptr<Interface::DB::Destination> db(
          new Interface::DB::Destination);

        db->Connect(Interface::DB::Destination::ORACLE,
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
        std::auto_ptr<Interface::DB::Destination> db(
          new Interface::DB::Destination);

        db->Connect(Interface::DB::Destination::POSTGRESQL,
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
        std::auto_ptr<IO::Stream> uc(build_unix_connector(i));

        if (config::interface::xml == i.protocol)
          dest = new Interface::XML::Destination(uc.get());
        else
          dest = new Interface::NDO::Destination(uc.get());
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
Interface::Source* factory::build_source(config::interface const& i)
{
  std::auto_ptr<Interface::NDO::Source> source;
  std::auto_ptr<IO::Stream> stream;

  stream.reset(build_stream(i));
  source.reset(new Interface::NDO::Source(stream.get()));
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
Interface::SourceDestination* factory::build_sourcedestination(
  config::interface const& i)
{
  std::auto_ptr<Interface::SourceDestination> sd;
  std::auto_ptr<IO::Stream> stream(build_stream(i));

  if (stream.get())
    {
      sd.reset(new NDO::SourceDestination(stream.get()));
      stream.release();
    }
  return (sd.release());
}
