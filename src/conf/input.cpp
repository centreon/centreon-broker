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

#include "conf/input.h"

using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Input constructor.
 *
 *  Initialize members to their default values.
 */
Input::Input()
  : type_(Input::UNKNOWN), port_(0), compress_(false), tls_(false) {}

/**
 *  \brief Input copy constructor.
 *
 *  Copy all parameters from the given object to the current instance.
 *
 *  \param[in] input Object to copy data from.
 */
Input::Input(const Input& input)
{
  this->operator=(input);
}

/**
 *  Input destructor.
 */
Input::~Input() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all parameters from the given object to the current instance.
 *
 *  \param[in] input Object to copy data from.
 *
 *  \return *this
 */
Input& Input::operator=(const Input& input)
{
  this->ca_ = input.ca_;
  this->cert_ = input.cert_;
  this->compress_ = input.compress_;
  this->key_ = input.key_;
  this->name_ = input.name_;
  this->tls_ = input.tls_;
  this->type_ = input.type_;
  switch (input.type_)
    {
     case IPV4:
     case IPV6:
      this->interface_ = input.interface_;
      this->port_ = input.port_;
      break ;
     case UNIX:
      this->socket_path_ = input.socket_path_;
      break ;
     default:
      ;
    }
  return (*this);
}

/**
 *  \brief Overload of the equal to operator.
 *
 *  Check if the two objects are equal. First the types are compared, then the
 *  names and finally specific values. If any comparison is false, the return
 *  value will be false.
 *
 *  \param[in] input Object to check against the current instance.
 *
 *  \return True if the two objects are equal, false otherwise.
 */
bool Input::operator==(const Input& input) const
{
  bool match;

  if ((this->type_ == input.type_)
      && (this->name_ == input.name_)
      && (this->ca_ == input.ca_)
      && (this->cert_ == input.cert_)
      && (this->compress_ == input.compress_)
      && (this->key_ == input.key_)
      && (this->tls_ == input.tls_))
    {
      switch (this->type_)
	{
	case IPV4:
	case IPV6:
	  match = ((this->interface_ == input.interface_)
                   && (this->port_ == input.port_));
	  break ;
	case UNIX:
	  match = (this->socket_path_ == input.socket_path_);
	  break ;
	default:
	  match = true;
	}
    }
  else
    match = false;
  return (match);
}

/**
 *  Overload of the not equal to operator.
 *
 *  \return The complement of the return value of operator==.
 */
bool Input::operator!=(const Input& input) const
{
  return (!(*this == input));
}

/**
 *  Overload of the less than operator.
 *
 *  \param[in] input Object to compare to.
 *
 *  \return true if *this is less than input, false otherwise.
 */
bool Input::operator<(const Input& input) const
{
  bool ret;

  if (this->type_ != input.type_)
    ret = (this->type_ < input.type_);
  else if (this->compress_ != input.compress_)
    ret = input.compress_;
  else if (this->tls_ != input.tls_)
    ret = input.tls_;
  else if (this->name_ != input.name_)
    ret = (this->name_ < input.name_);
  else if (this->ca_ != input.ca_)
    ret = (this->ca_ < input.ca_);
  else if (this->cert_ != input.cert_)
    ret = (this->cert_ < input.cert_);
  else if (this->key_ != input.key_)
    ret = (this->key_ < input.key_);
  else if ((IPV4 == this->type_) || (IPV6 == this->type_))
    {
      if (this->port_ != input.port_)
	ret = (this->port_ < input.port_);
      else if (this->interface_ != input.interface_)
	ret = (this->interface_ < input.interface_);
      else
	ret = false;
    }
  else if (UNIX == this->type_)
    {
      if (this->socket_path_ == input.socket_path_)
	ret = (this->socket_path_ < input.socket_path_);
      else
	ret = false;
    }
  else
    ret = false;
  return (ret);
}

/**
 *  Get the interface on which the input IP object should listen.
 *
 *  \return The IP address of the interface on which the input IP object should
 *          listen (empty for all).
 *
 *  \see SetIPInterface
 */
const std::string& Input::GetIPInterface() const throw ()
{
  return (this->interface_);
}

/**
 *  Get the port on which the input IP object should listen.
 *
 *  \return The port on which the input IP object should listen.
 *
 *  \see SetIPPort
 */
unsigned short Input::GetIPPort() const throw ()
{
  return (this->port_);
}

/**
 *  Get the name of the input.
 *
 *  \return The name of the input.
 *
 *  \see SetName
 */
const std::string& Input::GetName() const throw ()
{
  return (this->name_);
}

/**
 *  Determines whether or not TLS should be activated on the input.
 *
 *  \return true if TLS has to be activated.
 */
bool Input::GetTLS() const throw ()
{
  return (this->tls_);
}

/**
 *  Get the path of the trusted Certificate Authority certificate used for
 *  client authentication.
 *
 *  \return Path to the trusted CA's certificate.
 *
 *  \see SetTLSCA
 */
const std::string& Input::GetTLSCA() const throw ()
{
  return (this->ca_);
}

/**
 *  Get the path of the public certificate to use for encryption.
 *
 *  \return Path of the public certificate.
 *
 *  \see SetTLSCert
 */
const std::string& Input::GetTLSCert() const throw ()
{
  return (this->cert_);
}

/**
 *  Determines whether or not the TLS layer should provide compression as well
 *  as encryption.
 *
 *  \return true if compression is requested, false otherwise.
 *
 *  \see SetTLSCompress
 */
bool Input::GetTLSCompress() const throw ()
{
  return (this->compress_);
}

/**
 *  Get the path of the private key to use for decryption.
 *
 *  \return Path of the private key.
 *
 *  \see SetTLSKey
 */
const std::string& Input::GetTLSKey() const throw ()
{
  return (this->key_);
}

/**
 *  \brief Get the type of the input.
 *
 *  The type of the input determines which parameters are available.
 *
 *  \return The type of the input.
 *
 *  \see SetType
 */
Input::Type Input::GetType() const throw ()
{
  return (this->type_);
}

/**
 *  \brief Get the Unix domain socket path.
 *
 *  In case of an Input of type UNIX, get the path of the socket that should be
 *  used.
 *
 *  \return The Unix domain socket path.
 */
const std::string& Input::GetUnixSocketPath() const throw ()
{
  return (this->socket_path_);
}

/**
 *  Set the interface on which the input IP object should listen.
 *
 *  \param[in] iface The IP address of the interface on which the input IP
 *                   object shoud listen (empty for all).
 *
 *  \see GetIPInterface
 */
void Input::SetIPInterface(const std::string& iface)
{
  this->interface_ = iface;
  return ;
}

/**
 *  Set the port on which the input IP object shoud listen.
 *
 *  \param[in] port The port on which the input IP object should listen.
 *
 *  \see GetIPPort
 */
void Input::SetIPPort(unsigned short port) throw ()
{
  this->port_ = port;
  return ;
}

/**
 *  Set the name of the input.
 *
 *  \param[in] name The name of the input.
 *
 *  \see GetName
 */
void Input::SetName(const std::string& name)
{
  this->name_ = name;
  return ;
}

/**
 *  Set whether or not TLS should be activated on the input.
 *
 *  \param[in] true if TLS has to be activated, false otherwise.
 */
void Input::SetTLS(bool tls) throw ()
{
  this->tls_ = tls;
  return ;
}

/**
 *  Set the path of the trusted Certificate Authority certificate used for
 *  client authentication.
 *
 *  \param[in] ca Path to the trusted CA's certificate.
 *
 *  \see GetTLSCA
 */
void Input::SetTLSCA(const std::string& ca)
{
  this->ca_ = ca;
  return ;
}

/**
 *  Set the path of the public certificate to use for encryption.
 *
 *  \param[in] cert Path of the public certificate.
 *
 *  \see GetTLSCert
 */
void Input::SetTLSCert(const std::string& cert)
{
  this->cert_ = cert;
  return ;
}

/**
 *  Set whether or not the TLS layer should provide compression as well as
 *  encryption.
 *
 *  \param[in] compress true if compression if requested, false otherwise.
 *
 *  \see GetTLSCompress
 */
void Input::SetTLSCompress(bool compress) throw ()
{
  this->compress_ = compress;
  return ;
}

/**
 *  Set the path of the private key to use for decryption.
 *
 *  \param[in] Path of the private key.
 *
 *  \see GetTLSKey
 */
void Input::SetTLSKey(const std::string& key)
{
  this->key_ = key;
  return ;
}

/**
 *  Set the type of the input.
 *
 *  \param[in] type The type of the input.
 *
 *  \see GetType
 */
void Input::SetType(Input::Type type) throw ()
{
  this->type_ = type;
  return ;
}

/**
 *  Set the Unix domain socket path.
 *
 *  \param[in] usp The Unix domain socket path.
 *
 *  \see GetUnixSocketPath
 */
void Input::SetUnixSocketPath(const std::string& usp)
{
  this->socket_path_ = usp;
  return ;
}
