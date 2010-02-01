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

#include <algorithm>
#include <utility>
#include "concurrency/lock.h"
#include "events/events.h"
#include "multiplexing/internal.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"

using namespace Multiplexing;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Get the ID of the host.
 */
static void SetHostID(Events::HostServiceStatus* hss)
{
  std::map<std::pair<std::string, std::string>, int>::iterator it;
  Concurrency::Lock lock(gl_hostsm);

  it = gl_hosts.find(std::make_pair(hss->instance, hss->host));
  if (it != gl_hosts.end())
    hss->host_id = it->second;
  return ;
}

/**
 *  Get the ID of the service.
 */
static void SetServiceID(Events::ServiceStatus* ss)
{
  std::map<std::pair<std::pair<std::string, std::string>, std::string>, int>::iterator it;
  Concurrency::Lock lock(gl_servicesm);

  it = gl_services.find(std::make_pair(std::make_pair(ss->instance, ss->host),
                                       ss->service));
  if (it != gl_services.end())
    ss->service_id = it->second;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Publisher default constructor.
 */
Publisher::Publisher() {}

/**
 *  \brief Publisher copy constructor.
 *
 *  As Publisher does not hold any data value, this constructor is similar to
 *  the default constructor.
 *
 *  \param[in] publisher Unused.
 */
Publisher::Publisher(const Publisher& publisher)
  : Interface::Destination(publisher) {}

/**
 *  Publisher destructor.
 */
Publisher::~Publisher() {}

/**
 *  \brief Assignment operator overload.
 *
 *  As Publisher does not hold any data value, this assignment operator does
 *  nothing.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] publisher Unused.
 *
 *  \return *this
 */
Publisher& Publisher::operator=(const Publisher& publisher)
{
  this->Interface::Destination::operator=(publisher);
  return (*this);
}

/**
 *  \brief Prevent any event to be sent without error.
 *
 *  In theory Close() should prevent any event to be sent through the
 *  Event(Events::Event*) method without error. However for performance
 *  purposes, no check is actually performed and therefore this method
 *  does nothing.
 *  \par Safety No throw guarantee.
 */
void Publisher::Close()
{
  return ;
}

/**
 *  \brief Send an event to all subscribers.
 *
 *  As soon as the method returns, the Event object is owned by the Publisher,
 *  meaning that it'll be automatically destroyed when necessary.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] event Event to publish.
 */
void Publisher::Event(Events::Event* event)
{
  std::list<Subscriber*>::iterator end;
  Concurrency::Lock lock(gl_subscribersm);

  // Check event type.
  switch (event->GetType())
    {
     case Events::Event::HOST:
      {
	Events::Host* host;
	Concurrency::Lock lock(gl_hostsm);

	host = static_cast<Events::Host*>(event);
	gl_hosts[std::make_pair(host->instance, host->host)]
          = host->host_id;
      }
      break ;
     case Events::Event::HOSTSTATUS:
      SetHostID(static_cast<Events::HostServiceStatus*>(event));
      break ;
     case Events::Event::SERVICE:
      {
	Events::Service* service;
	Concurrency::Lock lock(gl_servicesm);

	service = static_cast<Events::Service*>(event);
	gl_services[std::make_pair(std::make_pair(service->instance,
                                                  service->host),
                                   service->service)] = service->service_id;
        SetHostID(static_cast<Events::HostServiceStatus*>(event));
      }
      break ;
     case Events::Event::SERVICESTATUS:
      SetHostID(static_cast<Events::HostServiceStatus*>(event));
      SetServiceID(static_cast<Events::ServiceStatus*>(event));
      break ;
     default:
      ; // Avoid compiler warning.
    }

  // Send object to every subscriber.
  end = gl_subscribers.end();
  for (std::list<Subscriber*>::iterator it = gl_subscribers.begin();
       it != end;
       ++it)
    {
      event->AddReader();
      (*it)->Event(event);
    }
  // Self deregistration.
  event->RemoveReader();

  return ;
}
