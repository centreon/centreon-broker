/*
**  Copyright 2010 MERETHIS
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

#include "config/logger.hh"

using namespace config;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members to this object.
 *
 *  @param[in] l Object to copy from.
 */
void logger::_internal_copy(logger const& l)
{
  _level  = l._level;
  _name   = l._name;
  _type   = l._type;
  _types  = l._types;
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
logger::logger()
  : _level(logging::HIGH),
    _type(unknown),
    _types(-1) {}

/**
 *  Copy constructor.
 *
 *  @param[in] l Object to copy from.
 */
logger::logger(logger const& l)
{
  _internal_copy(l);
}

/**
 *  Destructor.
 */
logger::~logger() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] l Object to copy from.
 *
 *  @return This object.
 */
logger& logger::operator=(logger const& l)
{
  _internal_copy(l);
  return (*this);
}

/**
 *  Equality operator overload.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if objects are equivalent, false otherwise.
 */
bool logger::operator==(logger const& l) const
{
  bool ret;

  ret = ((_level == l._level)
	 && (_type == l._type)
	 && (_types == l._types));
  if (ret && ((file == _type) || (standard == _type)))
    ret = (ret && (_name == l._name));
  return (ret);
}

/**
 *  Non-equality operator overload.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if objects are different, false otherwise.
 */
bool logger::operator!=(logger const& l) const
{
  return (!(*this == l));
}

/**
 *  Stricly inferior operator overload.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if current logger is stricly inferior to the argument.
 */
bool logger::operator<(logger const& l) const
{
  bool ret;

  if (_level != l._level)
    ret = (_level < l._level);
  else if (_type != l._type)
    ret = (_type < l._type);
  else if (_types != l._types)
    ret = (_types < l._types);
  else if (((file == _type) || (standard == _type))
	   && (_name != l._name))
    ret = (_name < l._name);
  else
    ret = false;
  return (ret);
}

/**
 *  Set the config parameter.
 *
 *  @param[in] c New value.
 */
void logger::config(bool c)
{
  if (c)
    _types = (_types | logging::CONFIG);
  else
    _types = (_types & ~logging::CONFIG);
  return ;
}

/**
 *  Get the config parameter.
 *
 *  @return Current value.
 */
bool logger::config() const
{
  return (_types & logging::CONFIG);
}

/**
 *  Set the debug parameter.
 *
 *  @param[in] d New value.
 */
void logger::debug(bool d)
{
  if (d)
    _types = (_types | logging::DEBUG);
  else
    _types = (_types & ~logging::DEBUG);
  return ;
}

/**
 *  Get the debug parameter.
 *
 *  @return Current value.
 */
bool logger::debug() const
{
  return (_types & logging::DEBUG);
}

/**
 *  Set the error parameter.
 *
 *  @param[in] e New value.
 */
void logger::error(bool e)
{
  if (e)
    _types = (_types | logging::ERROR);
  else
    _types = (_types & ~logging::ERROR);
  return ;
}

/**
 *  Get the error parameter.
 *
 *  @return Current value.
 */
bool logger::error() const
{
  return (_types & logging::ERROR);
}

/**
 *  Set the info parameter.
 *
 *  @param[in] i New value.
 */
void logger::info(bool i)
{
  if (i)
    _types = (_types | logging::INFO);
  else
    _types = (_types & ~logging::INFO);
  return ;
}

/**
 *  Get the info parameter.
 *
 *  @return Current value.
 */
bool logger::info() const
{
  return (_types & logging::INFO);
}

/**
 *  Set the level parameter.
 *
 *  @param[in] l New value.
 */
void logger::level(logging::level l)
{
  _level = l;
  return ;
}

/**
 *  Get the level parameter.
 *
 *  @return Current value.
 */
logging::level logger::level() const
{
  return (_level);
}

/**
 *  Set the name parameter.
 *
 *  @param[in] n New value.
 */
void logger::name(std::string const& n)
{
  _name = n;
  return ;
}

/**
 *  Get the name parameter.
 *
 *  @return Current value.
 */
std::string const& logger::name() const
{
  return (_name);
}

/**
 *  Set the type parameter.
 *
 *  @param[in] lt New value.
 */
void logger::type(logger::logger_type lt)
{
  _type = lt;
  return ;
}

/**
 *  Get the type parameter.
 *
 *  @return Current value.
 */
logger::logger_type logger::type() const
{
  return (_type);
}

/**
 *  Set the types parameter.
 *
 *  @param[in] t New value.
 */
void logger::types(unsigned int t)
{
  _types = t;
  return ;
}

/**
 *  Get the types parameter.
 *
 *  @return Current value.
 */
unsigned int logger::types() const
{
  return (_types);
}
