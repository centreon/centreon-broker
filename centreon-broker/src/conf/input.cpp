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

#include <cstring>
#include "conf/input.h"

using namespace CentreonBroker::Conf;

/**
 *  Input constructor.
 */
Input::Input()
{
  memset(this->ushorts_, 0, sizeof(this->ushorts_));
}

/**
 *  Input copy constructor.
 */
Input::Input(const Input& input)
{
  this->operator=(input);
}

/**
 *  Input destructor.
 */
Input::~Input()
{
}

/**
 *  Input operator= overload.
 */
Input& Input::operator=(const Input& input)
{
  memcpy(this->ushorts_, input.ushorts_, sizeof(this->ushorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = input.strings_[i];
  return (*this);
}

/**
 *  Input operator== overload.
 */
bool Input::operator==(const Input& input)
{
  bool match;

  match = (this->ushorts_[PORT] == input.ushorts_[PORT]);
  for (unsigned int i = 0; i < STRING_NB; i++)
    match = (match && (this->strings_[i] == input.strings_[i]));
  return (match);
}

/**
 *  Returns the port of the socket input.
 */
unsigned short Input::GetPort() const throw ()
{
  return (this->ushorts_[PORT]);
}

/**
 *  Get the Certificate Authority verification file used to verify the peer.
 */
const std::string& Input::GetTlsCa() const throw ()
{
  return (this->strings_[TLS_CA]);
}

/**
 *  Get the certificate that will be used by the client to encrypt the
 *  connection.
 */
const std::string& Input::GetTlsCertificate() const throw ()
{
  return (this->strings_[TLS_CERTIFICATE]);
}

/**
 *  Get the private key file used to decrypt the connection.
 */
const std::string& Input::GetTlsKey() const throw ()
{
  return (this->strings_[TLS_KEY]);
}

/**
 *  Returns the type of the output (file or socket).
 */
const std::string& Input::GetType() const throw ()
{
  return (this->strings_[TYPE]);
}

/**
 *  Set the port on which the socket should listen.
 */
void Input::SetPort(unsigned short port) throw ()
{
  this->ushorts_[PORT] = port;
  return ;
}

/**
 *  Set the CA file used to verify the peer.
 */
void Input::SetTlsCa(const std::string& ca)
{
  this->strings_[TLS_CA] = ca;
  return ;
}

/**
 *  Set the certificate that will be used by the client to encrypt the
 *  connection.
 */
void Input::SetTlsCertificate(const std::string& certificate)
{
  this->strings_[TLS_CERTIFICATE] = certificate;
  return ;
}

/**
 *  Set the private key that will be used to decrypt the connection.
 */
void Input::SetTlsKey(const std::string& key)
{
  this->strings_[TLS_KEY] = key;
  return ;
}

/**
 *  Set the type of the input (file or socket).
 */
void Input::SetType(const std::string& type)
{
  this->strings_[TYPE] = type;
  return ;
}
