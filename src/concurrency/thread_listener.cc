/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include "concurrency/thread_listener.hh"

using namespace concurrency;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
thread_listener::thread_listener() {}

/**
 *  @brief Copy constructor.
 *
 *  As thread_listener does not hold any data member, the copy
 *  constructor does nothing.
 *
 *  @param[in] tl Unused.
 */
thread_listener::thread_listener(thread_listener const& tl) {
  (void)tl;
}

/**
 *  @brief Assigment operator.
 *
 *  As thread_listener does not hold any data member, the assignment
 *  operator does nothing.
 *
 *  @param[in] tl Unused.
 *
 *  @return This object.
 */
thread_listener& thread_listener::operator=(thread_listener const& tl) {
  (void)tl;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
thread_listener::~thread_listener() {}

/**
 *  Method called on thread creation.
 *
 *  @param[in] t Thread being created.
 */
void thread_listener::on_create(thread* t) {
  (void)t;
  return ;
}

/**
 *  Method called on thread exit.
 *
 *  @param[in] t Thread being exited.
 */
void thread_listener::on_exit(thread* t) {
  (void)t;
  return ;
}

/**
 *  Method called on an arbitrary thread failure.
 *
 *  @param[in] t Failing thread,
 */
void thread_listener::on_failure(thread* t) {
  (void)t;
  return ;
}
