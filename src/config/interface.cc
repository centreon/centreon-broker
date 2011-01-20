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

#include "config/interface.hh"

using namespace config;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy data members.
 *
 *  Copy all data members from the given object to the current instance. This
 *  method is used by the copy constructor and the assignment operator.
 *  @par Safety Basic exception safety.
 *
 *  @param[in] i Object to copy.
 */
void interface::_internal_copy(interface const& i) {
  if (i.failover.get())
    failover.reset(new interface(*i.failover));
  db = i.db;
  failover_name = i.failover_name;
  filename = i.filename;
  host = i.host;
  name = i.name;
  net_iface = i.net_iface;
  password = i.password;
  port = i.port;
  protocol = i.protocol;
  socket = i.socket;
  type = i.type;
  user = i.user;
#ifdef USE_TLS
  ca = i.ca;
  cert = i.cert;
  compress = i.compress;
  key = i.key;
  tls = i.tls;
#endif /* USE_TLS */
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
interface::interface()
  : port(5668),
    protocol(ndo),
    type(unknown)
#ifdef USE_TLS
    ,compress(false)
    ,tls(false)
#endif /* USE_TLS */
{}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
interface::interface(interface const& i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
interface::~interface() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
interface& interface::operator=(interface const& i) {
  failover.reset();
  _internal_copy(i);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if objects are equivalent, false otherwise.
 */
bool interface::operator==(interface const& i) const {
  bool ret;
  if (type == i.type)
    switch (type) {
     case file:
      ret = ((filename == i.filename)
             && (protocol == i.protocol)
             && ((!failover.get() && !(i.failover.get()))
                 || (failover.get() && i.failover.get()
                     && (*failover == *i.failover))
                 ));
      break ;
     case ipv4_client:
     case ipv4_server:
     case ipv6_client:
     case ipv6_server:
      ret = ((host == i.host)
             && (net_iface == i.net_iface)
             && (port == i.port)
             && (protocol == i.protocol)
#ifdef USE_TLS
             && (tls == i.tls)
             && (!tls || ((ca == i.ca)
                          && (cert == i.cert)
                          && (compress == i.compress)
                          && (key == i.key)))
#endif /* USE_TLS */
             && ((!failover.get() && !(i.failover.get()))
                 || (failover.get()
                     && i.failover.get()
                     && (*failover == *i.failover))));
      break ;
     case mysql:
     case oracle:
     case postgresql:
      ret = ((db == i.db)
             && (host == i.host)
             && (password == i.password)
             && (user == i.user)
             && ((!failover.get() && !(i.failover.get()))
                 || (failover.get()
                     && i.failover.get()
                     && (*failover == *i.failover))));
      break ;
     case unix_client:
     case unix_server:
      ret = ((socket == i.socket)
#ifdef USE_TLS
             && (tls == i.tls)
             && (!tls || ((ca == i.ca)
                                && (cert == i.cert)
                                && (compress == i.compress)
                                && (key == i.key)))
#endif /* USE_TLS */
             && ((!failover.get() && !(i.failover.get()))
                 || (failover.get()
                     && i.failover.get()
                     && (*failover == *i.failover))));
      break ;
     default:
      ret = true;
    }
  else
    ret = false;
  return (ret);
}

/**
 *  Non-equality operator.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if objects are different, false otherwise.
 */
bool interface::operator!=(interface const& i) const {
  return (!this->operator==(i));
}

/**
 *  Strictly inferior operator.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if current instance is strictly inferior to the argument.
 */
bool interface::operator<(interface const& i) const {
  bool ret;
  if (type == i.type)
    switch (type) {
     case file:
      if (filename != i.filename)
        ret = (filename < i.filename);
      else if (failover.get() && i.failover.get())
        ret = (*failover < *i.failover);
      else
        ret = false;
      break ;
     case ipv4_client:
     case ipv4_server:
     case ipv6_client:
     case ipv6_server:
      if (host != i.host)
        ret = (host < i.host);
      else if (net_iface != i.net_iface)
        ret = (net_iface < i.net_iface);
      else if (port != i.port)
        ret = (port < i.port);
      else if (protocol != i.protocol)
        ret = (protocol < i.protocol);
#ifdef USE_TLS
      else if (tls != i.tls)
        ret = (tls < i.tls);
      else if (ca != i.ca)
        ret = (ca < i.ca);
      else if (cert != i.cert)
        ret = (cert < i.cert);
      else if (compress != i.compress)
        ret = (compress < i.compress);
      else if (key != i.key)
        ret = (key < i.key);
#endif /* USE_TLS */
      else if (failover.get() && i.failover.get())
        ret = (*failover < *i.failover);
      else
        ret = false;
      break ;
     case mysql:
     case oracle:
     case postgresql:
      if (db != i.db)
        ret = (db < i.db);
      else if (host != i.host)
        ret = (host < i.host);
      else if (password != i.password)
        ret = (password < i.password);
      else if (user != i.user)
        ret = (user < i.user);
      else if (failover.get() && i.failover.get())
        ret = (*failover < *i.failover);
      else
        ret = false;
      break ;
     case unix_client:
     case unix_server:
      if (socket != i.socket)
        ret = (socket < i.socket);
#ifdef USE_TLS
      else if (tls != i.tls)
        ret = (tls < i.tls);
      else if (ca != i.ca)
        ret = (ca < i.ca);
      else if (cert != i.cert)
        ret = (cert < i.cert);
      else if (compress != i.compress)
        ret = (compress < i.compress);
      else if (key != i.key)
        ret = (key < i.key);
#endif /* USE_TLS */
      else if (failover.get() && i.failover.get())
        ret = (*failover < *i.failover);
      else
        ret = false;
      break ;
     default:
      ret = false;
    }
  else
    ret = (type < i.type);
  return (ret);
}
