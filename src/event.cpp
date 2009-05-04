/*
** event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#include "event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Event copy constructor.
 */
Event::Event(const Event& event)
{
  (void)event;
}

/**
 *  Event operator= overload.
 */
Event& Event::operator=(const Event& event)
{
  (void)event;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Event constructor.
 */
Event::Event()
{
}

/**
 *  Event destructor.
 */
Event::~Event()
{
}
