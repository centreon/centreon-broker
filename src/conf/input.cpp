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
Input::Input() : type_(Input::UNKNOWN), port_(0) {}

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
  this->name_ = input.name_;
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
      && (this->name_ == input.name_))
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
 *  \return this->GetName() < input.GetName()
 */
bool Input::operator<(const Input& input) const
{
  return (this->name_ < input.name_);
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
