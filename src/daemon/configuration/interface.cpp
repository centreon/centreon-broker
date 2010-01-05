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

#include "configuration/interface.h"

using namespace Configuration;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy data members.
 *
 *  Copy all data members from the given object to the current instance. This
 *  method is used by the copy constructor and the assignment operator.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] interface Object to copy data from.
 */
void Interface::InternalCopy(const Interface& interface)
{
  this->db        = interface.db;
  this->filename  = interface.filename;
  this->host      = interface.host;
  this->interface = interface.interface;
  this->name      = interface.name;
  this->password  = interface.password;
  this->port      = interface.port;
  this->protocol  = interface.protocol;
  this->socket    = interface.socket;
  this->type      = interface.type;
  this->user      = interface.user;
#ifdef USE_TLS
  this->ca        = interface.ca;
  this->cert      = interface.cert;
  this->compress  = interface.compress;
  this->key       = interface.key;
  this->tls       = interface.tls;
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
Interface::Interface()
{
  this->port     = 5668;
  this->protocol = NDO;
  this->type     = UNKNOWN_TYPE;
#ifdef USE_TLS
  this->compress = false;
  this->tls      = false;
#endif /* USE_TLS */
}

/**
 *  Interface copy constructor.
 *
 *  \param[in] interface Object to copy data from.
 */
Interface::Interface(const Interface& interface)
{
  this->InternalCopy(interface);
}

/**
 *  Interface destructor.
 */
Interface::~Interface() {}

/**
 *  Assignment operator overload.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] interface Object to copy data from.
 *
 *  \return *this
 */
Interface& Interface::operator=(const Interface& interface)
{
  this->InternalCopy(interface);
  return (*this);
}

/**
 *  Equality operator overload.
 *
 *  \param[in] interface Interface to compare to.
 *
 *  \return true if objects are equivalent, false otherwise.
 */
bool Interface::operator==(const Interface& interface) const
{
  bool ret;

  if (this->type == interface.type)
    switch (this->type)
      {
       case FILE:
        ret = ((this->filename == interface.filename)
               && (this->protocol == interface.protocol));
        break ;
       case IPV4_CLIENT:
       case IPV4_SERVER:
       case IPV6_CLIENT:
       case IPV6_SERVER:
        ret = ((this->host == interface.host)
               && (this->interface == interface.interface)
               && (this->port == interface.port)
               && (this->protocol == interface.protocol)
#ifdef USE_TLS
               && (this->tls == interface.tls)
               && (!this->tls || ((this->ca == interface.ca)
                                  && (this->cert == interface.cert)
                                  && (this->compress == interface.compress)
                                  && (this->key == interface.key)))
#endif /* USE_TLS */
              );
        break ;
       case MYSQL:
       case ORACLE:
       case POSTGRESQL:
        ret = ((this->db == interface.db)
               && (this->host == interface.host)
               && (this->password == interface.password)
               && (this->user == interface.user));
        break ;
       case UNIX_CLIENT:
       case UNIX_SERVER:
        ret = ((this->socket == interface.socket)
#ifdef USE_TLS
               && (this->tls == interface.tls)
               && (!this->tls || ((this->ca == interface.ca)
                                  && (this->cert == interface.cert)
                                  && (this->compress == interface.compress)
                                  && (this->key == interface.key)))
#endif /* USE_TLS */
              );
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
 *  \param[in] interface Interface to compare to.
 *
 *  \return true if objects are different, false otherwise.
 */
bool Interface::operator!=(const Interface& interface) const
{
  return (!this->operator==(interface));
}

/**
 *  Strictly inferior operator overload.
 *
 *  \param[in] interface Interface to compare to.
 *
 *  \return true if current instance is strictly inferior to the argument.
 */
bool Interface::operator<(const Interface& interface) const
{
  bool ret;

  if (this->type == interface.type)
    switch (this->type)
      {
       case FILE:
        ret = (this->filename < interface.filename);
        break ;
       case IPV4_CLIENT:
       case IPV4_SERVER:
       case IPV6_CLIENT:
       case IPV6_SERVER:
        if (this->host != interface.host)
          ret = (this->host < interface.host);
        else if (this->interface != interface.interface)
          ret = (this->interface < interface.interface);
        else if (this->port != interface.port)
          ret = (this->port < interface.port);
        else if (this->protocol != interface.protocol)
          ret = (this->protocol < interface.protocol);
#ifdef USE_TLS
        else if (this->tls != interface.tls)
          ret = (this->tls < interface.tls);
        else if (this->ca != interface.ca)
          ret = (this->ca < interface.ca);
        else if (this->cert != interface.cert)
          ret = (this->cert < interface.cert);
        else if (this->compress != interface.compress)
          ret = (this->compress < interface.compress);
        else if (this->key != interface.key)
          ret = (this->key < interface.key);
#endif /* USE_TLS */
        else
          ret = false;
        break ;
       case MYSQL:
       case ORACLE:
       case POSTGRESQL:
        if (this->db != interface.db)
          ret = (this->db < interface.db);
        else if (this->host != interface.host)
          ret = (this->host < interface.host);
        else if (this->password != interface.password)
          ret = (this->password < interface.password);
        else
          ret = (this->user < interface.user);
        break ;
       case UNIX_CLIENT:
       case UNIX_SERVER:
        if (this->socket != interface.socket)
          ret = (this->socket < interface.socket);
#ifdef USE_TLS
        else if (this->tls != interface.tls)
          ret = (this->tls < interface.tls);
        else if (this->ca != interface.ca)
          ret = (this->ca < interface.ca);
        else if (this->cert != interface.cert)
          ret = (this->cert < interface.cert);
        else if (this->compress != interface.compress)
          ret = (this->compress < interface.compress);
        else if (this->key != interface.key)
          ret = (this->key < interface.key);
#endif /* USE_TLS */
        else
          ret = false;
        break ;
       default:
        ret = false;
      }
  else
    ret = (this->type < interface.type);
  return (ret);
}
