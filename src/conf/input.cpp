/*
** input.cpp for CentreonBroker in ./src/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/17/09 Matthieu Kermagoret
** Last update 06/17/09 Matthieu Kermagoret
*/

#include <cstring>
#include "conf/input.h"

using namespace CentreonBroker::Conf;

/**
 *  Input constructor.
 */
Input::Input()
{
  memset(this->bools_, 0, sizeof(this->bools_));
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
  memcpy(this->bools_, input.bools_, sizeof(this->bools_));
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
  return ((input.bools_[TLS] == this->bools_[TLS])
          && (input.ushorts_[PORT] == this->ushorts_[PORT])
          && (input.strings_[TYPE] == this->strings_[TYPE]));
}

/**
 *  Returns the port of the socket input.
 */
unsigned short Input::GetPort() const throw ()
{
  return (this->ushorts_[PORT]);
}

/**
 *  Determines whether or not TLS should be activated on the socket input.
 */
bool Input::GetTls() const throw ()
{
  return (this->bools_[TLS]);
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
 *  Activate TLS on the socket.
 */
void Input::SetTls(bool tls) throw ()
{
  this->bools_[TLS] = tls;
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
