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

#include <assert.h>
#include <stdlib.h>         // for abort
#include "input/listener.h"

using namespace Input;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Listener copy constructor.
 *
 *  Listener is not copyable. Therefore any attempt to use the copy constructor
 *  will result in a call to abort().
 *
 *  \param[in] listener Unused.
 */
Listener::Listener(const Listener& listener)
{
  (void)listener;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Listener is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  \param[in] listener Unused.
 *
 *  \return *this
 */
Listener& Listener::operator=(const Listener& listener)
{
  (void)listener;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Listener default constructor.
 */
Listener::Listener() {}

/**
 *  Listener destructor.
 */
Listener::~Listener()
{
  // XXX : kill thread if necessary
}

/**
 *  \brief Overload of the parenthesis operator.
 *
 *  This method is used as the entry point of the processing thread which
 *  listens on incoming connections.
 *  \param[in] No throw guarantee.
 *
 *  \param[in] acceptor Acceptor on which connections will be listened.
 */
void Listener::operator()()
{
  try
    {
      std::auto_ptr<IO::Stream> stream;

      stream.reset(this->acceptor_->Accept());
      while (stream.get())
        {
	  // XXX : add the NDO/XML object
	  stream.reset(this->acceptor_->Accept());
        }
    }
  catch (...) {}
  return ;
}

/**
 *  \brief Launch processing thread.
 *
 *  Launch the thread waiting on incoming connections. Upon successful return
 *  from this method, the acceptor will be owned by the Listener.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] acceptor Acceptor on which incoming clients will be awaited.
 */
void Listener::Init(IO::Acceptor* acceptor)
{
  this->acceptor_.reset(acceptor);
  try
    {
      this->thread_.Run(this);
    }
  catch (...)
    {
      this->acceptor_.release();
      throw ;
    }
  return ;
}
