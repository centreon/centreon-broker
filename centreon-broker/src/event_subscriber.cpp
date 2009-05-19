/*
** event_subscriber.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#include "event_publisher.h"
#include "event_subscriber.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  EventSubscriber constructor.
 */
EventSubscriber::EventSubscriber()
{
  EventPublisher::GetInstance()->Subscribe(this);
}

/**
 *  EventSubscriber copy constructor.
 */
EventSubscriber::EventSubscriber(const EventSubscriber& es)
{
  (void)es;
  EventPublisher::GetInstance()->Subscribe(this);
}

/**
 *  EventSubscriber destructor.
 */
EventSubscriber::~EventSubscriber()
{
  EventPublisher::GetInstance()->Unsubscribe(this);
}

/**
 *  EventSubscriber operator= overload.
 */
EventSubscriber& EventSubscriber::operator=(const EventSubscriber& es)
{
  (void)es;
  return (*this);
}
