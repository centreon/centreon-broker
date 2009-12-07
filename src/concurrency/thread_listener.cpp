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

#include "concurrency/thread_listener.h"

using namespace Concurrency;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  ThreadListener default constructor.
 */
ThreadListener::ThreadListener() {}

/**
 *  \brief ThreadListener copy constructor.
 *
 *  As ThreadListener does not hold any data member, the copy constructor does
 *  nothing.
 *
 *  \param[in] tl Unused.
 */
ThreadListener::ThreadListener(const ThreadListener& tl)
{
  (void)tl;
}

/**
 *  \brief Assigment operator overload.
 *
 *  As ThreadListener does not hold any data member, the assignment operator
 *  does nothing.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] tl Unused.
 *
 *  \return *this
 */
ThreadListener& ThreadListener::operator=(const ThreadListener& tl)
{
  (void)tl;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ThreadListener destructor.
 */
ThreadListener::~ThreadListener() {}

/**
 *  Method called on thread creation.
 *
 *  \param[in] thread Thread being created.
 */
void ThreadListener::OnCreate(Thread* thread)
{
  (void)thread;
  return ;
}

/**
 *  Method called on thread exit.
 *
 *  \param[in] thread Thread being exited.
 */
void ThreadListener::OnExit(Thread* thread)
{
  (void)thread;
  return ;
}
