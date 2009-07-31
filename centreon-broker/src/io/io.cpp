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

#include "io/io.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 Stream                                      *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief Stream copy constructor.
 *
 *  As Stream is just an interface with no data, the copy constructor does
 *  nothing and therefore cannot generate any exception. Also, as users of this
 *  class should not call the copy constructor directly, but instead through
 *  concrete objects, the copy constructor is declared protected.
 *
 *  \param[in] stream Object to copy from (unused).
 */
Stream::Stream(const Stream& stream) throw ()
{
  (void)stream;
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  As Stream is just an interface with no data, the operator= method does
 *  nothing and therefore cannot generate any exception. Also, as users of this
 *  class should not directly assign a Stream object to another, the method is
 *  declared protected. If one want to copy a Stream object, he'll have to go
 *  through the concrete object's interface.
 *
 *  \param[in] stream Object to copy from (unused).
 *
 *  \return *this.
 */
Stream& Stream::operator=(const Stream& stream) throw ()
{
  (void)stream;
  return (*this);
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  \brief Stream default constructor.
 *
 *  As Stream is just an interface with no data, the default constructor does
 *  nothing and therefore cannot generate any exception.
 */
Stream::Stream() throw () {}

/**
 *  \brief Stream destructor.
 *
 *  Does nothing directly but beware the overriden destructors.
 */
Stream::~Stream() {}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                Acceptor                                     *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief Acceptor copy constructor.
 *
 *  As Acceptor is just an interface with no data, the copy constructor does
 *  nothing and therefore cannot generate any exception. Also, as users of this
 *  class should not call the copy constructor directly, but instead through
 *  concrete objects, the copy constructor is declared protected.
 *
 *  \param[in] acceptor Object to copy from (unused).
 */
Acceptor::Acceptor(const Acceptor& acceptor) throw ()
{
  (void)acceptor;
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  As Acceptor is just an interface with no data, the operator= method does
 *  nothing and therefore cannot generate any exception. Also, as users of this
 *  class should not directly assign an Acceptor object to another, the method
 *  is declared protected. If one want to copy an Acceptor object, he'll have
 *  to go through the concrete object's interface.
 *
 *  \param[in] acceptor Object to copy from (unused).
 *
 *  \return *this
 */
Acceptor& Acceptor::operator=(const Acceptor& acceptor) throw ()
{
  (void)acceptor;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Acceptor default constructor.
 *
 *  As Acceptor is just an interface with no data, the default constructor does
 *  nothing and therefore cannot generate any exception.
 */
Acceptor::Acceptor() throw () {}

/**
 *  \brief Acceptor destructor.
 *
 *  Does nothing directly, but beware the overrident destructors.
 */
Acceptor::~Acceptor() {}
