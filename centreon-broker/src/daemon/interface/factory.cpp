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

#include "interface/factory.h"

using namespace Interface;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Factory default constructor.
 */
Factory::Factory() {}

/**
 *  Factory copy constructor.
 *
 *  \param[in] factory Unused.
 */
Factory::Factory(const Factory& factory)
{
  (void)factory;
}

/**
 *  Factory destructor.
 */
Factory::~Factory() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] factory Unused.
 *
 *  \return *this
 */
Factory& Factory::operator=(const Factory& factory)
{
  (void)factory;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Build a destination from its configuration.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] i Configuration of the new destination.
 *
 *  \return A destination matching the configuration.
 *
 *  \throw Exception Destination creation failed.
 */
Destination::Destination* Factory::Destination(
                            const Configuration::Interface& i)
{
}

/**
 *  \brief Get the Interface instance.
 *
 *  Interface is a singleton. This method returns the only available static
 *  instance of this class.
 *  \par Safety No throw guarantee.
 *
 *  \return The Interface instance.
 */
Factory& Factory::Instance()
{
  static Factory factory;

  return (factory);
}

/**
 *  Build a source from its configuration.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] i Configuration of the new destination.
 *
 *  \return A source matching the configuration.
 *
 *  \throw Exception Source creation failed.
 */
Source::Source* Factory::Source(const Configuration::Interface& i)
{
}

/**
 *  Build a source-destination from its configuration.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] i Configuration of the new destination.
 *
 *  \return A source-destination matching the configuration.
 *
 *  \throw Exception Source-destination creation failed.
 */
SourceDestination* Factory::SourceDestination(
                     const Configuration::Interface& i)
{
}
