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
 *  @param[in] i Object to copy data from.
 */
void interface::_internal_copy(interface const& i)
{
  if (i.failover.get())
    this->failover.reset(new interface(*i.failover));
  this->db            = i.db;
  this->failover_name = i.failover_name;
  this->filename      = i.filename;
  this->host          = i.host;
  this->name          = i.name;
  this->net_iface     = i.net_iface;
  this->password      = i.password;
  this->port          = i.port;
  this->protocol      = i.protocol;
  this->socket        = i.socket;
  this->type          = i.type;
  this->user          = i.user;
#ifdef USE_TLS
  this->ca            = i.ca;
  this->cert          = i.cert;
  this->compress      = i.compress;
  this->key           = i.key;
  this->tls           = i.tls;
#endif /* USE_TLS */
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Interface default constructor.
 */
interface::interface()
{
  this->port     = 5668;
  this->protocol = ndo;
  this->type     = unknown;
#ifdef USE_TLS
  this->compress = false;
  this->tls      = false;
#endif /* USE_TLS */
}

/**
 *  Interface copy constructor.
 *
 *  @param[in] i Object to copy data from.
 */
interface::interface(interface const& i)
{
  this->_internal_copy(i);
}

/**
 *  Interface destructor.
 */
interface::~interface() {}

/**
 *  Assignment operator overload.
 *  @par Safety Basic exception safety.
 *
 *  @param[in] i Object to copy data from.
 *
 *  @return *this
 */
interface& interface::operator=(interface const& i)
{
  this->failover.reset();
  this->_internal_copy(i);
  return (*this);
}

/**
 *  Equality operator overload.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if objects are equivalent, false otherwise.
 */
bool interface::operator==(interface const& i) const
{
  bool ret;

  if (this->type == i.type)
    switch (this->type)
      {
       case file:
        ret = ((this->filename == i.filename)
               && (this->protocol == i.protocol)
               && ((!this->failover.get() && !(i.failover.get()))
                   || (this->failover.get() && i.failover.get()
                       && (*this->failover == *i.failover))
                   ));
        break ;
       case ipv4_client:
       case ipv4_server:
       case ipv6_client:
       case ipv6_server:
        ret = ((this->host == i.host)
               && (this->net_iface == i.net_iface)
               && (this->port == i.port)
               && (this->protocol == i.protocol)
#ifdef USE_TLS
               && (this->tls == i.tls)
               && (!this->tls || ((this->ca == i.ca)
                                  && (this->cert == i.cert)
                                  && (this->compress == i.compress)
                                  && (this->key == i.key)))
#endif /* USE_TLS */
               && ((!this->failover.get() && !(i.failover.get()))
                   || (this->failover.get() && i.failover.get()
                       && (*this->failover == *i.failover))));
        break ;
       case mysql:
       case oracle:
       case postgresql:
        ret = ((this->db == i.db)
               && (this->host == i.host)
               && (this->password == i.password)
               && (this->user == i.user)
               && ((!this->failover.get() && !(i.failover.get()))
                   || (this->failover.get() && i.failover.get()
                       && (*this->failover == *i.failover))));
        break ;
       case unix_client:
       case unix_server:
        ret = ((this->socket == i.socket)
#ifdef USE_TLS
               && (this->tls == i.tls)
               && (!this->tls || ((this->ca == i.ca)
                                  && (this->cert == i.cert)
                                  && (this->compress == i.compress)
                                  && (this->key == i.key)))
#endif /* USE_TLS */
               && ((!this->failover.get() && !(i.failover.get()))
                   || (this->failover.get() && i.failover.get()
                       && (*this->failover == *i.failover))));
        break ;
       default:
        ret = true;
      }
  else
    ret = false;
  return (ret);
}

/**
 *  Non-equality operator overload.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if objects are different, false otherwise.
 */
bool interface::operator!=(interface const& i) const
{
  return (!this->operator==(i));
}

/**
 *  Strictly inferior operator overload.
 *
 *  @param[in] i Interface to compare to.
 *
 *  @return true if current instance is strictly inferior to the argument.
 */
bool interface::operator<(interface const& i) const
{
  bool ret;

  if (this->type == i.type)
    switch (this->type)
      {
       case file:
        if (this->filename != i.filename)
          ret = (this->filename < i.filename);
        else if (this->failover.get() && i.failover.get())
          ret = (*this->failover < *i.failover);
        else
          ret = false;
        break ;
       case ipv4_client:
       case ipv4_server:
       case ipv6_client:
       case ipv6_server:
        if (this->host != i.host)
          ret = (this->host < i.host);
        else if (this->net_iface != i.net_iface)
          ret = (this->net_iface < i.net_iface);
        else if (this->port != i.port)
          ret = (this->port < i.port);
        else if (this->protocol != i.protocol)
          ret = (this->protocol < i.protocol);
#ifdef USE_TLS
        else if (this->tls != i.tls)
          ret = (this->tls < i.tls);
        else if (this->ca != i.ca)
          ret = (this->ca < i.ca);
        else if (this->cert != i.cert)
          ret = (this->cert < i.cert);
        else if (this->compress != i.compress)
          ret = (this->compress < i.compress);
        else if (this->key != i.key)
          ret = (this->key < i.key);
#endif /* USE_TLS */
        else if (this->failover.get() && i.failover.get())
          ret = (*this->failover < *i.failover);
        else
          ret = false;
        break ;
       case mysql:
       case oracle:
       case postgresql:
        if (this->db != i.db)
          ret = (this->db < i.db);
        else if (this->host != i.host)
          ret = (this->host < i.host);
        else if (this->password != i.password)
          ret = (this->password < i.password);
        else if (this->user != i.user)
          ret = (this->user < i.user);
        else if (this->failover.get() && i.failover.get())
          ret = (*this->failover < *i.failover);
        else
          ret = false;
        break ;
       case unix_client:
       case unix_server:
        if (this->socket != i.socket)
          ret = (this->socket < i.socket);
#ifdef USE_TLS
        else if (this->tls != i.tls)
          ret = (this->tls < i.tls);
        else if (this->ca != i.ca)
          ret = (this->ca < i.ca);
        else if (this->cert != i.cert)
          ret = (this->cert < i.cert);
        else if (this->compress != i.compress)
          ret = (this->compress < i.compress);
        else if (this->key != i.key)
          ret = (this->key < i.key);
#endif /* USE_TLS */
        else if (this->failover.get() && i.failover.get())
          ret = (*this->failover < *i.failover);
        else
          ret = false;
        break ;
       default:
        ret = false;
      }
  else
    ret = (this->type < i.type);
  return (ret);
}
